#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdint.h>
#include "buzzer.h"
#include "lpuart2.h"
void completed_onEntry(void) 
{ 
    gameActiv = false;
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
    gameActiv = false;
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
    buzzer_set(false);
    send_run_data_to_esp();
}
void reset_onUpdate(void) 
{ 
    FSM_addEvent(E_RESET_COMPLETE);
    displayLoadTemplate(RESET_D,0 , false);
}
void reset_onExit(void) 
{ 
    #if DEBUG_ON_PC

    printf("[");
    for(uint8_t i = 0; i < getNumRooms(); i++) 
    {
        float minutes = runData.roomTimes[i];
        uint16_t totalSec = (uint16_t)(minutes * 60.0f);
        uint16_t min = totalSec / 60;
        uint16_t sec = totalSec % 60;
        printf("%u:%02u, ", min, sec);
    }
    printf("]\n");
   
    printf("Aantal fouten: %d\n",runData.wrongAnswerCount);
    
    float minutes = runData.totalTime;
    uint16_t totalSec = (uint16_t)(minutes * 60.0f);
    uint16_t min = totalSec / 60;
    uint16_t sec = totalSec % 60;
    printf("Start Tijd: %u:%02u\n",min, sec);
    printf("Moelijkhijd: %d\n",runData.difficulty);
    printf("Max Rooms: %d\n",runData.maxRooms);

    #endif
}
