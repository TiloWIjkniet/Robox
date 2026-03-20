#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include <stdio.h>
#include "hexDisplay.h"
#define EXIT_DEV_CODE "0000"

void idle_onEntry(void)
{
       
    emptyInputBuffer();
    setMapCoordinates((uint8_t[]){INVALID_COORD, INVALID_COORD});
    displayLoadTemplate(IDLE_D, 0, true);
    
}
void idle_onUpdate(void)
{
    updateInputBuffer();
    if(!hasNewAnswer) return;
    hasNewAnswer = false;

    if(isInputMatching(answerBuffer, EXIT_DEV_CODE)) FSM_addEvent(E_START_DEV); 
    
    FSM_addEvent(E_START_GAME); 
}
void idle_onExit(void)
{

}