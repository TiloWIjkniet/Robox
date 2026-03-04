#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdint.h>
#include <stdio.h>

void completed_onEntry(void) 
{ 

}
void completed_onUpdate(void) 
{ 
    bool displayFinished = false;
    displayFinished = displayLoadTemplate(GEHAALT_D, 5 * 1000, false);

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
    displayFinished = displayLoadTemplate(TIJD_D, 5 * 1000, false);

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
    displayLoadTemplate(RESET_D,0 , false);
}
void reset_onExit(void) 
{ 

    for(uint8_t i = 0; i < getNumRooms(); i++) {
        uint16_t minutes = (uint16_t)runData.roomTimes[i];        // hele minuten
        uint16_t seconds = (uint16_t)((runData.roomTimes[i] - minutes) * 60); // rest seconden
        printf("    room %d: %u:%02u ", i, minutes, seconds);
         printf("\n");
    }
   
    printf("Aantal fouten: %d\n",runData.wrongAnswerCount);
    printf("Start Tijd: %d\n",runData.totalTime);
    printf("Moelijkhijd: %d\n",runData.difficulty);
    printf("Max Rooms: %d\n",runData.maxRooms);


}
