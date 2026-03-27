#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include "touch_sensor.h"
#include "lpuart1.h"
#include "time_millis.h"
#include "hexDisplay.h"

#define WEBSERVER_ON  0xCC
#define WEBSERVER_OFF 0xEE
#define START_BYTE_GET_SETTINGS_DATA 0xBB
#define START_BYTE_SEND_RUN_DATA 0xAA
#define START_BYTE_GLOBAL_DATA 0xAA
#define START_BYTE_ROOM_DATA   0xAB

#define EXIT_DEV_CODE "0000"
#define OPEN_ALL_COMPARTMETS "9999"

#define TIMEOUT_MS 500  

#define RETRY_ATTEMPTS 5
#define MAX_DIFFICULTY 5

globalSettings_t globalSettings =
{
    WRONG_ANSWER_MINUS_5MIN_STOP,
    60,
    AUDIO_ON
};
runData_t runData = 
{
    .roomTimes = {20.5, 10.5, 10,8,2},
    .wrongAnswerCount = 5,
    .totalTime = 60,
    .difficulty = 3,
    .maxRooms = 51
};

roomSettings_t roomsSettings[MAX_ROOMS];
const char ROOM_CODES[20][2] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"};

void dev_page_onEntry(void)
{
    lpuart1_putchar(WEBSERVER_ON); //Zet webserver aan
    forceDisplayTemplate(D_DEV_PAGE, 0);
    displayDigits(OFF,DEV_MODE_HEX_DIS,OFF,DEV_MODE_HEX_DIS,OFF);
    emptyInputBuffer();
}

void dev_page_onUpdate(void)
{
    updateInputBuffer();
    if(!hasNewAnswer) return;

    if(isInputMatching(answerBuffer, EXIT_DEV_CODE))
    {
        FSM_addEvent(E_EXIT_DEV); 
        return;
    }
    else if(isInputMatching(answerBuffer, OPEN_ALL_COMPARTMETS))
    {
        openCompartment(NON_C);
        //TODO: Moet nog ge implementeerd worden
    }

    uint8_t numberOfRooms = getNumRooms();
    for(int room = 0; room < numberOfRooms; room++)
    {
        if(isInputMatching(answerBuffer, ROOM_CODES[room]))
        {   
            receive_room_settings_from_esp();                                   //Verkrijg ingestelde data van esp
            setMapCoordinates(roomsSettings[room].coordinates);                 //Zet de codinate van de geslecteerde kamer aan
            hexDisplay_setTime(0,room);
            //TODO: Dispalys de settings voor die spesefieke room 
            break;
        }
    }
}

void dev_page_onExit(void)
{
    receive_room_settings_from_esp();   //Verkrijg ingestelde data van esp
    lpuart1_putchar(WEBSERVER_OFF);     // zet web servber uit 
}

/**
 * @brief Ontvangt globale instellingen en room settings van de ESP via UART1.
 *
 * Deze functie gebruikt een eenvoudige state-machine om eerst de globale instellingen
 * (`globalSettings`) te lezen en vervolgens de instellingen per kamer (`roomsSettings[]`).
 * 
 * State-machine:
 * - WAIT_GLOBAL_START: wacht op startbyte globale data
 * - READ_GLOBAL: lees bytes van globale instellingen
 * - WAIT_ROOM_START: wacht op startbyte room data
 * - READ_ROOM: lees bytes van een enkele kamer
 *
 * Timeouts:
 * - Als er langer dan TIMEOUT_MS geen byte binnenkomt, wordt de state gereset
 *   naar WAIT_GLOBAL_START en retourneert de functie false.
 *
 * @return true  Als alle globale en room settings correct zijn ontvangen
 * @return false Als een timeout plaatsvond of niet alle data werd ontvangen
 *
 * @note Deze functie blokkeert totdat data beschikbaar is of timeout optreedt.
 * @note Ontvanger (ESP) moet dezelfde struct layout gebruiken.
 */
bool receive_room_settings(void)
{
    static enum {WAIT_GLOBAL_START, READ_GLOBAL, WAIT_ROOM_START, READ_ROOM} state = WAIT_GLOBAL_START;
    static size_t byteIndex = 0;
    static uint8_t room = 0;
    static uint8_t* p;

    uint32_t timeoutStart = millis();
    while(true)
    {

        //Controleerd of er een time oud is
        if(millis() - timeoutStart > TIMEOUT_MS)
        {
            state = WAIT_GLOBAL_START;
            return false;
        }

        if(lpuart1_rxcnt() <= 0) continue; // wacht op data
        
        uint8_t b = lpuart1_getchar();
        timeoutStart = millis();
        switch(state)
        {
            case WAIT_GLOBAL_START:
                if(b == START_BYTE_GLOBAL_DATA)
                {
                    p = (uint8_t*)&globalSettings;
                    byteIndex = 0;
                    state = READ_GLOBAL;
                }
                break;

            case READ_GLOBAL:
                p[byteIndex++] = b;
                if(byteIndex >= sizeof(globalSettings))
                {
                    state = WAIT_ROOM_START;
                    byteIndex = 0;
                    room = 0;
                    globalSettings.difficulty = (globalSettings.difficulty - 1) % MAX_DIFFICULTY; //zorg dat moeilijkheid tussen 0 en 4 is
                }
                break;

            case WAIT_ROOM_START:
                if(b == START_BYTE_ROOM_DATA)
                {
                    p = (uint8_t*)&roomsSettings[room];
                    byteIndex = 0;
                    state = READ_ROOM;
                }
                break;

            case READ_ROOM:
                p[byteIndex++] = b;
                if(byteIndex >= sizeof(roomSettings_t))
                {
                    room++;
                    if(room >= MAX_ROOMS)
                    {
                        state = WAIT_GLOBAL_START;
                        return true; 
                    }
                    else
                    {
                        state = WAIT_ROOM_START;
                    }
                }
                break;
        }
    }

    return false; // nooit bereikt
}

/**
 * @brief Ontvangt de room settings van de ESP via UART1.
 *
 * Deze functie probeert maximaal RETRY_ATTEMPTS keer de room settings van de ESP
 * op te halen. Hiervoor wordt eerst een startbyte gestuurd om de ESP te laten weten
 * dat er om settings gevraagd wordt.
 *
 * De functie gebruikt de helperfunctie `receive_room_settings()` om daadwerkelijk
 * de data te lezen. Als na RETRY_ATTEMPTS pogingen nog geen data ontvangen is,
 * wordt een kritieke fout weergegeven op het display en stopt de MCU in een oneindige loop.
 *
 *
 * Belangrijk:
 * - `RETRY_ATTEMPTS` moet gedefinieerd zijn en bepaalt hoe vaak er gepoogd wordt
 *   data te ontvangen voordat een fout wordt gemeld.
 * - `displayLoadTemplate()` wordt gebruikt om een foutmelding te tonen.
 * - Deze functie blokkeert totdat data succesvol is ontvangen of de kritieke fout optreedt.
 *
 * @note Zorg dat de ESP dezelfde protocollen en structuren gebruikt voor room settings.
 */
void receive_room_settings_from_esp(void)
{   
    //Probeer 5x data van esp te verkijgen, als dit niet lukt disply kritiche error
    uint8_t attempts = 0;
    do
    {
        attempts ++;
        if(attempts > RETRY_ATTEMPTS)
        {
            forceDisplayTemplate(D_ERROR, 10000); // Toon kritische error
            displayDigits(ERROR_HEX_DIS,ERROR_HEX_DIS,ERROR_HEX_DIS,ERROR_HEX_DIS,OFF);
            while(true);
        }
        lpuart1_putchar(START_BYTE_GET_SETTINGS_DATA);

    }while(!receive_room_settings());

    // printf("Moelijkhijd: %d\n", globalSettings.difficulty);
    // printf("tijd: %d\n",        globalSettings.totalTime);
    // printf("audio: %d\n", globalSettings.audio);

    // printf("\nroomSettings\n");
    // for(int i = 0; i < getNumRooms(); i++)
    // {
    //     printf("coordinates: %d, %d\n", roomsSettings[i].coordinates[0], roomsSettings[i].coordinates[1]);
    //     printf("beconIp: %s\n",        roomsSettings[i].beconIp);
    //     printf("answers: ");
    //     for(int x = 0; x < MAX_ANSWERS; x++) printf("%s, ",roomsSettings[i].answers[x]);
    //     printf("\n");
    //     printf("openCompartment: %d\n", roomsSettings[i].openCompartment);
    //     printf("specialActies: %d\n", roomsSettings[i].specialActies);
    //     printf("kamer naam: %s\n", roomsSettings[i].naam);
    //     printf("\n");
    // }

}

/**
 * @brief Verstuurt de runData struct naar de ESP via UART1.
 *
 * Belangrijk:
 * - De ontvanger (ESP) moet hetzelfde struct-formaat kennen en dezelfde volgorde gebruiken.
 * - START_BYTE_SEND_RUN_DATA dient als synchronisatiebyte zodat de ESP weet dat
 *   er runData begint.
 *
 * @note Deze functie blokkeert totdat alle bytes zijn verzonden via lpuart1_putchar().
 */
void send_run_data_to_esp(void)
{
    // IDEA: Stuur verkrijg een alle data ontvangen bit van esp ander retry
    uint8_t *data = (uint8_t*)&runData;
    size_t size = sizeof(runData);

    lpuart1_putchar(START_BYTE_SEND_RUN_DATA);
    
    for (size_t i = 0; i < size; i++)
    {
        lpuart1_putchar(data[i]);
    }
}