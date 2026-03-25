#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include <stdio.h>
#include "hexDisplay.h"

#define ENTER_DEV_CODE "0000"

void idle_onEntry(void)
{
    emptyInputBuffer();
    setMapCoordinates((uint8_t[]){INVALID_COORD, INVALID_COORD});
    forceDisplayTemplate(D_IDLE, 0);
    hexDisplay_setTime(0,0);
}
void idle_onUpdate(void)
{
    updateInputBuffer();
    if(!hasNewAnswer) return;

    if(isInputMatching(answerBuffer, ENTER_DEV_CODE)) {FSM_addEvent(E_START_DEV); return;} 
    
    FSM_addEvent(E_START_GAME); 
}
void idle_onExit(void)
{

}