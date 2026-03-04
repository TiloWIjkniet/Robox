#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"

#define EXIT_DEV_CODE "0000"
#define DISPLAY_SETTINGS_CODE "CODE"


globalSettings_t globalSettings =
{
    3,
    60,
    true
};
runData_t runData;
roomSettings_t roomsSettings[MAX_ROOMS] =
{
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"0000"},NON_C, NON_S},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"1111"},NON_C, NON_S},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"2222"},NON_C, NON_S},
    {{0,0}, "BECON IP VAN DICHTSBIJZIJNDE BEACON", {"3333"},NON_C, NON_S},
};

uint8_t getNumRooms(void) 
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < ARRAY_SIZE(roomsSettings); i++) {
        if(roomsSettings[i].beconIp[0] == '\0') break;
        count++;
    }
    return count -1;
}

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
    }


}

void dev_page_onExit(void)
{
    //Sla ontvange data op in epromp
}

