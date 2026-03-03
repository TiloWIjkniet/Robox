#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"

void completed_onEntry(void) 
{ 

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
    //Save data to eprmp
}
void reset_onUpdate(void) 
{ 
    FSM_addEvent(E_RESET_COMPLETE);
}
void reset_onExit(void) 
{ 

}
