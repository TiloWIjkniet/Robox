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
#include "audio.h"
#include <string.h>
#include <stdlib.h>
#include "HM10.h"

#define WEBSERVER_ON  0xCC
#define WEBSERVER_OFF 0xEE
#define START_BYTE_GET_SETTINGS_DATA 0xBB
#define START_BYTE_SEND_RUN_DATA 0xAA
#define START_BYTE_GLOBAL_DATA 0xAA
#define START_BYTE_ROOM_DATA   0xAB
#define NEW_DATA_BYTE 0xDD
#define BYTE_SEND_RUN_DATA_START 0xC1
#define BYTE_SEND_RUN_DATA_ROOM  0xC2
#define BYTE_SEND_RUN_DATA_END   0xC3
#define START_BYTE_ASK_RUN_DATA  0x11

#define EXIT_DEV_CODE "0000"
#define OPEN_ALL_COMPARTMETS "3333"
#define SET_AUDIO_VOLUME "1111"
#define RECEIVE_ROOM_SETTINGS "2222"
#define TIMEOUT_MS 500  

#define RETRY_ATTEMPTS 5
#define MAX_DIFFICULTY 5




globalSettings_t globalSettings =
{
    WRONG_ANSWER_MINUS_5MIN_STOP,
    10,
    AUDIO_OFF,
    LANGUAGE_ENGLISH,
    NOT_CENSORED
};
runData_t runData;
roomSettings_t roomsSettings[MAX_ROOMS] = 
{
    {{0,1}, "0000", {"0000"}, NON_C, KEY_S, "Room 1"},
    {{1,0}, "1111", {"1111"}, NON_C, SWITCH_S, "Room 2"},
    {{1,1}, "2222", {"2222"}, NON_C, NON_S, "Room 3"}
};
const char ROOM_CODES[20][2] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"};

bool newData = false;

void checkIfNewDataFromEsp(void);

void dev_page_onEntry(void)
{
    //Anable webserver on esp
    lpuart1_putchar(WEBSERVER_ON); 

    forceDisplayTemplate(D_DEV_PAGE, 0);
    displayDigits(DEV_MODE_HEX);
    emptyInputBuffer();
}

bool handleExit(void)
{
    if(isInputMatching(answerBuffer, EXIT_DEV_CODE))
    {
        //Exit dev mode
        FSM_addEvent(E_EXIT_DEV); 
        return true;
    }
    return false;
};
bool handleOpenAll(void)
{
    if(isInputMatching(answerBuffer, OPEN_ALL_COMPARTMETS))
    {
        //Open al compartments for testing/ resetting
        openCompartment(NON_C);
        playAudio(AUDIO_OPEN_COMPARTMENT);
        return true;
    }
    return false;
}
bool handleDebugBeacons(void)
{
    if(isInputMatching(answerBuffer, RECEIVE_ROOM_SETTINGS))
    {
        char lowestBeconsString[MAX_BEACONS_IN_LIST * 30]; 
        
        getLowestBecons(lowestBeconsString, sizeof(lowestBeconsString));
        printCustomDisplay(lowestBeconsString);
        return true;
    }
    return false;
}
bool handleVolume(void)
{
    char *pos = strstr(answerBuffer, SET_AUDIO_VOLUME);
    hasNewAnswer = false;
    if(pos != NULL)
    {
        //Set or get audio volume 
        pos += strlen(SET_AUDIO_VOLUME);
        if(*pos != '\0')
        {
            //If value is provided after command, set volume
            int value = atoi(pos);
            if (volume != value) audioSetVolume((uint8_t)value);
            volume = value;
        }

        //Display current volume
        uint8_t roomFirstVolume = volume / 10;
        uint8_t roomSecondVolume = volume % 10;
        displayDigitsValues(OFF,OFF,roomFirstVolume > 0 ? roomFirstVolume : OFF, roomSecondVolume, OFF);

        playGlobelAudio(AUDIO_AUDIO_CHECK);
        return true;
    }
    return false;
}
void handleRoomSelection(void)
{
    uint8_t numberOfRooms = getNumRooms();
    for(int room = 0; room < numberOfRooms; room++)
    {
        if(isInputMatching(answerBuffer, ROOM_CODES[room]))
        { 
            //Recive room settings from the esp  
            #if ESP_CANN
            if(newData) receive_room_settings_from_esp();   
            #endif    
            //Zet de codinate van de geslecteerde kamer aan                            
            setMapCoordinates(roomsSettings[room].coordinates);                
            
            //Displyt de kamer nummer op de hex display
            uint8_t roomFirstDigit = room / 10;
            uint8_t roomSecondDigit = room % 10;
            displayDigitsValues(OFF,OFF,roomFirstDigit > 0 ? roomFirstDigit : OFF, roomSecondDigit, OFF); //disply het kamernummer op de hex display
            //TODO: Dispalys de settings voor die spesefieke room 
            return;
        }
    }
}

void dev_page_onUpdate(void)
{
    checkIfNewDataFromEsp();
    if(!hasNewAnswer) return;
    
    if (handleExit()) return;
    if (handleOpenAll()) return;
    if (handleDebugBeacons()) return;
    if (handleVolume()) return;

    handleRoomSelection();
}

void dev_page_onExit(void)
{
    //Verkrijg ingestelde data van esp
    receive_room_settings_from_esp();  
    
    //Disble webserver on esp
    lpuart1_putchar(WEBSERVER_OFF); 
}

void checkIfNewDataFromEsp(void)
{
    if(newData) return;
    if(lpuart1_rxcnt() > 0)
    {
        char byte = lpuart1_getchar();
        if(byte == NEW_DATA_BYTE)
        {
            newData = true;
        } 
    }
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
            displayDigits(ERROR_HEX);
            
            while(true);
        }
        lpuart1_putchar(START_BYTE_GET_SETTINGS_DATA);

    }while(!receive_room_settings());
    
    newData = false;
}



void send_run_data_to_esp_start(void)
{
    lpuart1_putchar(START_BYTE_SEND_RUN_DATA);
    lpuart1_putchar(BYTE_SEND_RUN_DATA_START);

    lpuart1_putchar(runData.totalTime);
    lpuart1_putchar(runData.difficulty);
    lpuart1_putchar(runData.maxRooms);
}

void send_run_data_to_esp_room(void)
{
    lpuart1_putchar(START_BYTE_SEND_RUN_DATA);
    lpuart1_putchar(BYTE_SEND_RUN_DATA_ROOM);
    lpuart1_putchar(roomIndex);

    uint8_t *p = (uint8_t*)&runData.roomTimes[roomIndex];
    for(int i = 0; i < 4; i++)
    {
        lpuart1_putchar(p[i]);
    }
    lpuart1_putchar(runData.wrongAnswerCnt);
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


void send_run_data_to_esp_end(void)
{
    lpuart1_putchar(START_BYTE_SEND_RUN_DATA);
    lpuart1_putchar(BYTE_SEND_RUN_DATA_END);

    uint8_t *data = (uint8_t*)&runData;
    size_t size = sizeof(runData);
    
    for (size_t i = 0; i < size; i++)
    {
        lpuart1_putchar(data[i]);
    } 
}


 bool isGameBizy(uint8_t *rIndex)
 {
    lpuart1_putchar(0xFA);
    uint32_t startWait = millis();
    uint8_t atempts = 0;
    while(lpuart1_rxcnt() <= 0)
    {
        uint32_t now = millis();
        if(now - startWait > 1000)
        {
            lpuart1_putchar(0xFA);
            startWait = now;
            atempts ++;
            if(atempts >= 5)
            {
                return false;
            }
        } 
    }
    while(lpuart1_rxcnt() > 0)
    {
        *rIndex = lpuart1_getchar();
        if(*rIndex < 20) 
        {
            break;
        }
    }
    
    if(*rIndex != 0)
    {
        return true;
    }
    return false;
 }

 bool getRunData()
 {
    bool resefingData = false;
    size_t index = 0;
    uint32_t timeoutStart = millis();
    uint8_t *data = (uint8_t*)&runData;
    while(true)
    {

        //Controleerd of er een time oud is
        if(millis() - timeoutStart > TIMEOUT_MS)
        {
            return false;
        }
        if(lpuart1_rxcnt() <= 0) continue; // wacht op data

        char byte = lpuart1_getchar();
        if(!resefingData)
        {   
            if(byte == 0x12) resefingData = true;

            continue;
        }

        data[index++] = byte; 

        if (index >= sizeof(runData)) 
        {
            return true;
        }
        
    }
    return false; 
 }

 int askRunData()
 {
    
    for(uint8_t i = 0; i < 5; i ++)
    {
        while(lpuart1_rxcnt() > 0)
        {
            lpuart1_getchar();
        }
        lpuart1_putchar(START_BYTE_ASK_RUN_DATA);
        if(getRunData()) return 0;
    }
    return -1;
 }

 