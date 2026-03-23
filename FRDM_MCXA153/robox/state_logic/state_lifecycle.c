#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdint.h>
#include "buzzer.h"
#include "hexDisplay.h"
#include "time_millis.h"
#define BLINK_DURATION_DISARMT 250
#define BLINK_DURATION_TIMEOUT 150

void completed_onEntry(void) 
{ 
    gameActiv = false;
}
void completed_onUpdate(void) 
{ 
    static uint32_t lastBlink = 0;

    bool displayFinished = false;
    displayFinished = displayLoadTemplate(TIJD_D, 5 * 1000, false);


    uint32_t now = millis();
    if(now - lastBlink > BLINK_DURATION_DISARMT) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {
        lastBlink = now;
        if(displayFinished) hexDisplay_setTime(OFF, OFF);
        else updateGameTimer();
        displayFinished = !displayFinished;   
    }

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
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;

    bool displayFinished = false;
    displayFinished = displayLoadTemplate(TIJD_D, 5 * 1000, false);


    uint32_t now = millis();
    if(now - lastBlink > BLINK_DURATION_TIMEOUT) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {
        lastBlink = now;
        buzzer_play(BUZZERT_DURATION);
        hexDisplay_setTime(displayStatus ? OFF : 0, displayStatus ? OFF : 0);
        
    }


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
