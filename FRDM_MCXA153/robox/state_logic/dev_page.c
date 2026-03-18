#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include "touch_sensor.h"
#include "lupuart2.h"
#define EXIT_DEV_CODE "0000"
#define OPEN_ALL_COMPARTMETS "9999"

globalSettings_t globalSettings =
{
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,
    31,
    AUDIO_ON
};
runData_t runData;

runData_t runDatas[MAX_RUNS];

roomSettings_t roomsSettings[MAX_ROOMS] =
{
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"0000"},NON_C, TOUCH_SENSOR},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"1111"},NON_C, NON_S},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"2222"},NON_C, NON_S}

};

const char ROOM_CODES[20][2] = {"0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19"};

void dev_page_onEntry(void)
{
    emptyInputBuffer();
    displayLoadTemplate(DEF_D, 0, true);
    //Zet esp aan
    //Stuur run data naar esp
}

void dev_page_onUpdate(void)
{
    //Ontvang data van esp
    //Kijk of je uit dev mode gaat
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
        //Opent all compartments
        openCompartment(NON_C);
    }
    for(int room = 0; room < getNumRooms(); room++)
    {
        //Print unike rome setting
        if(isInputMatching(answerBuffer, ROOM_CODES[room]))
        {    
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

    displayLoadTemplate(GET_DATA_D, 0, true);
    receive_room_settings_from_esp();
}

void receive_room_settings_from_esp(void)
{
    
    
    uint8_t *data = (uint8_t*)roomsSettings;
    size_t size = sizeof(roomsSettings);

    lpuart2_putchar(0xBB);

    uint32_t tick = 0;
    while (lpuart2_getchar() != 0xAA) 
    {
        tick++;
        if(tick > 1000000) lpuart2_putchar(0xBB);
    } // Wacht tot start byte ontvangen wordt, time out na 1 miljoen ticks

    for (size_t i = 0; i < size; i++)
    {
        data[i] = lpuart2_getchar();
    }
}

void send_run_data_to_esp(void)
{
    uint8_t *data = (uint8_t*)runDatas;
    size_t size = sizeof(runDatas);

    lpuart2_putchar(0xAA);
    for (size_t i = 0; i < size; i++)
    {
        lpuart2_putchar(data[i]);
    }
}