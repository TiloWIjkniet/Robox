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

#define EXIT_DEV_CODE "0000"
#define OPEN_ALL_COMPARTMETS "9999"
#define TIMEOUT_MS 500  
globalSettings_t globalSettings =
{
    WRONG_ANSWER_MINUS_15MIN_STOP,
    60,
    AUDIO_ON
};
runData_t runData = 
{
    .roomTimes = {20.5, 10.5, 10,8,2},
    .wrongAnswerCount = 5,
    .totalTime = 60,
    .difficulty = 3,
    .maxRooms = 5
};

roomSettings_t roomsSettings[MAX_ROOMS] =
{
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"0000"},NON_C, TOUCH_SENSOR, "NAAM"},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"1111"},NON_C, NON_S,  "NAAM"},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"2222"},NON_C, NON_S,  "NAAM"}

};

const char ROOM_CODES[20][2] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"};

void dev_page_onEntry(void)
{
    lpuart1_putchar(0xCC); // zet webserver aan
    emptyInputBuffer();
    displayLoadTemplate(DEF_D, 0, true);
}

void dev_page_onUpdate(void)
{
    updateInputBuffer();
    if(!hasNewAnswer) return;
    hasNewAnswer = false;

    if(isInputMatching(answerBuffer, EXIT_DEV_CODE))
    {
        FSM_addEvent(E_EXIT_DEV); 
        return;
    }
    if(isInputMatching(answerBuffer, OPEN_ALL_COMPARTMETS))
    {

        openCompartment(NON_C);
    }
    for(int room = 0; room < getNumRooms(); room++)
    {
        if(isInputMatching(answerBuffer, ROOM_CODES[room]))
        {   
            receive_room_settings_from_esp();
            setMapCoordinates(roomsSettings[room].coordinates);
            #if DEBUG_ON_PC
    
            printf("[%d, %d]   ",roomsSettings[room].coordinates[0],roomsSettings[room].coordinates[0]);
            printf("%s   ",roomsSettings[room].beconIp);
            printf("[   ");
            for(int answer = 0; answer < MAX_ANSWERS; answer++)
            {
                printf("%s, ",roomsSettings[room].answers[answer]);
            }
            printf("]   ");
            printf("%d   ",roomsSettings[room].openCompartment);
            printf("%d\n",roomsSettings[room].specialActies);
            #endif
            return;
            
        }
    }

}

void dev_page_onExit(void)
{
    setMapCoordinates((uint8_t[]){INVALID_COORD, INVALID_COORD});

    receive_room_settings_from_esp();
    lpuart1_putchar(0xEE); // zet web servber uit 
}

bool receive_room_settings(void)
{
    static enum {WAIT_GLOBAL_START, READ_GLOBAL, WAIT_ROOM_START, READ_ROOM} state = WAIT_GLOBAL_START;
    static size_t byteIndex = 0;
    static uint8_t room = 0;
    static uint8_t* p;

    uint32_t timeoutStart = millis();

    while(lpuart1_rxcnt() > 0)
    {
        uint8_t b = lpuart1_getchar();

        switch(state)
        {
            case WAIT_GLOBAL_START:
                if(b == 0xAA)
                {
                    p = (uint8_t*)&globalSettings;
                    byteIndex = 0;
                    state = READ_GLOBAL;
                    timeoutStart = millis();
                    printf("Start globals\n");
                }
                break;

            case READ_GLOBAL:
                p[byteIndex++] = b;
                timeoutStart = millis(); // reset timeout per byte
                if(byteIndex >= sizeof(globalSettings))
                {
                    state = WAIT_ROOM_START;
                    byteIndex = 0;
                    room = 0;
                    printf("Globals ontvangen\n");
                }
                break;

            case WAIT_ROOM_START:
                if(b == 0xAB)
                {
                    p = (uint8_t*)&roomsSettings[room];
                    byteIndex = 0;
                    state = READ_ROOM;
                    timeoutStart = millis();
                }
                break;

            case READ_ROOM:
                p[byteIndex++] = b;
                timeoutStart = millis();
                if(byteIndex >= sizeof(roomSettings_t))
                {
                    room++;
                    if(room >= MAX_ROOMS)
                    {
                        state = WAIT_GLOBAL_START; // klaar
                        printf("Alle rooms ontvangen\n");
                        return true; 
                    }
                    else
                    {
                        state = WAIT_ROOM_START; // wacht op startbyte volgende room
                    }
                }
                break;
        }

        // check timeout
        if(millis() - timeoutStart > TIMEOUT_MS)
        {
            printf("Timeout!\n");
            state = WAIT_GLOBAL_START;
            return false;
        }
    }

    return false; // nog niet klaar
}

void receive_room_settings_from_esp(void)
{

    printf("get data\n");
    lpuart1_putchar(0xBB);
    while(!receive_room_settings());

    printf("Moelijkhijd: %d\n", globalSettings.difficulty);
    printf("tijd: %d\n",        globalSettings.totalTime);
    printf("audio: %d\n", globalSettings.audio);

    printf("\nroomSettings\n");
    for(int i = 0; i < getNumRooms(); i++)
    {
        printf("coordinates: %d, %d\n", roomsSettings[i].coordinates[0], roomsSettings[i].coordinates[1]);
        printf("beconIp: %s\n",        roomsSettings[i].beconIp);
        printf("answers: ");
        for(int x = 0; x < MAX_ANSWERS; x++) printf("%s, ",roomsSettings[i].answers[x]);
        printf("\n");
        printf("openCompartment: %d\n", roomsSettings[i].openCompartment);
        printf("specialActies: %d\n", roomsSettings[i].specialActies);
        printf("kamer naam: %s\n", roomsSettings[i].naam);
        printf("\n");
    }

}

void send_run_data_to_esp(void)
{
    uint8_t *data = (uint8_t*)&runData;
    size_t size = sizeof(runData);

    lpuart1_putchar(0xAA);

    for (size_t i = 0; i < size; i++)
    {
        lpuart1_putchar(data[i]);
    }
}