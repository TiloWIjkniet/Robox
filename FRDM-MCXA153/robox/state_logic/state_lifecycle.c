#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"

void completed_onEntry(void) 
{ 
    runData.finished = true;
}
void completed_onUpdate(void) 
{ 
    bool displayFinished = false;
    displayFinished = displayLoadTemplate(NON, 5 * 1000, false);

    if(!displayFinished) return;
    FSM_addEvent(E_GAME_COMPLETED);
}
void completed_onExit(void) 
{ 

}

void timeout_onEntry(void) 
{ 
    runData.finished = false;
}
void timeout_onUpdate(void) 
{ 
    bool displayFinished = false;
    displayFinished = displayLoadTemplate(NON, 5 * 1000, false);

    if(!displayFinished) return;
    FSM_addEvent(E_GAME_TIMEOUT);
}
void timeout_onExit(void) 
{ 

}

void reset_onEntry(void) 
{ 
    timeGamePenaltyMillis += timeGamePanaltyBuffer;
    timeGamePanaltyBuffer = 0;
    uint32_t elapsedTime = getElapsedTime();
    runData.totalTime = elapsedTime / 1000; // millis naar sec
}
void reset_onUpdate(void) 
{ 
    FSM_addEvent(E_RESET_COMPLETE);
}
void reset_onExit(void) 
{ 

}
