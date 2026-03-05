#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include <stdio.h>

#define EXIT_DEV_CODE "0000"
#define OPEN_ALL_COMPARTMETS "9999"

globalSettings_t globalSettings =
{
    4,
    10,
    true
};
runData_t runData;
roomSettings_t roomsSettings[MAX_ROOMS] =
{
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"0000"},NON_C, NON_S},
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
        openCompartment(NON_C);
    }
    for(int room = 0; room < getNumRooms(); room++)
    {
        if(isInputMatching(answerBuffer, ROOM_CODES[room]))
        {
            setMapCoordinates(roomsSettings[room].coordinates);

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
            return;
        }
    }

}

void dev_page_onExit(void)
{
    setMapCoordinates((uint8_t[]){INVALID_COORD, INVALID_COORD});
    //Sla ontvange data op in epromp
}

