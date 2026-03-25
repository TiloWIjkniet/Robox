#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdint.h>
#include "buzzer.h"
#include "hexDisplay.h"
#include "time_millis.h"
#define BLINK_DURATION_DISARMT 500
#define MAX_BLINK_DURATION_TIMEOUT 200
#define MIN_BLINK_DURATION_TIMEOUT 25

uint32_t startRoomTimeMillis = 0;

void completed_onEntry(void) 
{ 
    gameActiv = false;
    startRoomTimeMillis = millis();
    displayLoadTemplate(TIJD_D, 0, true);
}
void completed_onUpdate(void) 
{ 
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;


    uint32_t now = millis();
    if(now - lastBlink > BLINK_DURATION_DISARMT) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {
        lastBlink = now;
        if(displayStatus) updateGameTimer();
        else displayDigits(OFF, OFF, OFF, OFF, OFF);
        displayStatus = !displayStatus;   
    }

    if(now - startRoomTimeMillis < 5 * 1000) return; // wacht nog even met naar volgende scherm gaan zodat de spelers de tijd kunnen zien
    FSM_addEvent(E_GAME_COMPLETED);
}
void completed_onExit(void) 
{ 
}

void timeout_onEntry(void) 
{ 
    gameActiv = false;
    startRoomTimeMillis = millis();
    displayLoadTemplate(TIJD_D,0, true);

}
void timeout_onUpdate(void) 
{ 
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;
    static uint32_t blinkDuration = MAX_BLINK_DURATION_TIMEOUT;


    uint32_t now = millis();
    if(now - lastBlink > blinkDuration) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {

        float ratio = (float)(now - startRoomTimeMillis) /(float)((4 * 1000));
        if (ratio > 1.0f) ratio = 1.0f;
        blinkDuration = MAX_BLINK_DURATION_TIMEOUT - (MAX_BLINK_DURATION_TIMEOUT - MIN_BLINK_DURATION_TIMEOUT) * (ratio * ratio);
        lastBlink = now;
        buzzer_play(BUZZERT_DURATION);
        if(displayStatus) hexDisplay_setTime(0, 0);
        else displayDigits(OFF, OFF, OFF, OFF, OFF);
        displayStatus = !displayStatus;
        
    }

    if(now - startRoomTimeMillis < 5 * 1000) return; // wacht nog even met naar volgende scherm gaan zodat de spelers de tijd kunnen zien
    FSM_addEvent(E_GAME_TIMEOUT);
}
void timeout_onExit(void) 
{ 

}

void reset_onEntry(void) 
{ 
   
}
void reset_onUpdate(void) 
{ 
    FSM_addEvent(E_RESET_COMPLETE);
    displayLoadTemplate(RESET_D,0 , false);
}
void reset_onExit(void) 
{ 
    send_run_data_to_esp();
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
