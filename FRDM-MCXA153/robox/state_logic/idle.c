#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"

void idle_onEntry(void)
{
    setMapCoordinates((uint8_t[]){INVALID_COORD, INVALID_COORD});
    displayLoadTemplate(NON, 0, true);
}
void idle_onUpdate(void)
{
    //Kijk of dev mode enablet word
    //kijk of spel gestart word
    if(!hasNewAnswer) return;
    hasNewAnswer = false;

    if(isInputMatching(answerBuffer, EXIT_DEV_CODE)) FSM_addEvent(E_START_DEV); 
    
    FSM_addEvent(E_START_GAME); 
}
void idle_onExit(void)
{

}