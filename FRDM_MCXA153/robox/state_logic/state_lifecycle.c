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
#define END_SCREEN_DURATION 5000
#define FAST_BLINK_DURATION 1000
uint32_t startRoomTimeMillis = 0;

void completed_onEntry(void) 
{ 
    gameActiv = false;
    startRoomTimeMillis = millis();
    addDisplayTemplate(D_WIN, END_SCREEN_DURATION);
}
void completed_onUpdate(void) 
{ 
    //BUG soms blijft hij hier in een onijnige loop hangen
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;

    uint32_t now = millis();
    if(now - lastBlink > BLINK_DURATION_DISARMT) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {
        lastBlink = now;
        if(displayStatus) setGameTimer(getTimeRemaining() / 1000);
        else displayDigits(OFF, OFF, OFF, OFF, OFF);
        displayStatus = !displayStatus;   
    }

    if(now - startRoomTimeMillis < END_SCREEN_DURATION) return; // wacht nog even met naar volgende scherm gaan zodat de spelers de tijd kunnen zien

    FSM_addEvent(E_GAME_COMPLETED);
}
void completed_onExit(void) 
{ 
}

void timeout_onEntry(void) 
{ 
    gameActiv = false;
    startRoomTimeMillis = millis();
    addDisplayTemplate(D_TIME_UP, END_SCREEN_DURATION);

}
void timeout_onUpdate(void) 
{ 
    //BUG soms blijft hij hier in een onijnige loop hangen
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;
    static uint32_t blinkDuration = MAX_BLINK_DURATION_TIMEOUT;


    uint32_t now = millis();
    if(now - lastBlink > blinkDuration) // laat als de bom af gaat de buzzer pipen het het hex display knipperen
    {

        float ratio = (float)(now - startRoomTimeMillis) /(float)(END_SCREEN_DURATION - FAST_BLINK_DURATION);
        if (ratio > 1.0f) ratio = 1.0f;
        blinkDuration = MAX_BLINK_DURATION_TIMEOUT - (MAX_BLINK_DURATION_TIMEOUT - MIN_BLINK_DURATION_TIMEOUT) * (ratio * ratio);

        lastBlink = now;

        buzzer_play(BUZZERT_DURATION);

        if(displayStatus) hexDisplay_setTime(0, 0);
        else displayDigits(OFF, OFF, OFF, OFF, OFF);
        displayStatus = !displayStatus;
        
    }

    if(now - startRoomTimeMillis < END_SCREEN_DURATION) return; // wacht nog even met naar volgende scherm gaan zodat de spelers de tijd kunnen zien
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
}
void reset_onExit(void) 
{ 
    send_run_data_to_esp();
}
