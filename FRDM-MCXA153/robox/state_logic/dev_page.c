#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"


#define EXIT_DEV_CODE "CODE"
#define DISPLAY_SETTINGS_CODE "CODE"

void dev_page_onEntry(void)
{
    displayLoadTemplate(NON, 0, true);
    //Zet esp aan
    //Stuur run data naar esp
}

void dev_page_onUpdate(void)
{
    //Ontvang data van esp
    //Kijk of je uit dev mode gaat

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

