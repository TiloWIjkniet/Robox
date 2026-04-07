#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdint.h>
#include "buzzer.h"
#include "hexDisplay.h"
#include "time_millis.h"
#include "audio.h"
#define BLINK_DURATION_DISARMT 500
#define MAX_BLINK_DURATION_TIMEOUT 200
#define MIN_BLINK_DURATION_TIMEOUT 25
#define FAST_BLINK_DURATION 1000
uint32_t startRoomTimeMillis = 0;


void completed_onEntry(void) 
{ 
    //Ends the game 
    gameActiv = false;

    addDisplayTemplate(D_WIN, DISPLAY_5S);
    playAudio(AUDIO_BOM_HAS_BEEN_DEFUSED);

}
void completed_onUpdate(void) 
{ 
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;

    uint32_t now = millis();
    if(now - lastBlink > BLINK_DURATION_DISARMT) 
    {
        //makes the hex display blink between the time remaining  and emty screen when the game is completed
        lastBlink = now;
        if(displayStatus) setGameTimer(getTimeRemaining() / 1000); //Convert milliseconds to seconds for display
        else displayDigits(ALL_OFF_HEX);
        displayStatus = !displayStatus;   
    }

    //Waits until the win template is done playing before sending event to go to next screen
    if(!isDisplayTemplateDonePlaying()) return; 

    playAudio(AUDIO_BOM_HAS_BEEN_DEFUSED);
    FSM_addEvent(E_GAME_COMPLETED);
}
void completed_onExit(void) 
{ 
}

void timeout_onEntry(void) 
{ 
    //Ends the game
    gameActiv = false;
    startRoomTimeMillis = millis();
    addDisplayTemplate(D_TIME_UP, DISPLAY_5S);
}
void timeout_onUpdate(void) 
{
    static uint32_t lastBlink = 0;
    static bool displayStatus = false;
    static uint32_t blinkDuration = MAX_BLINK_DURATION_TIMEOUT;

    uint32_t now = millis();
    //Blinks the hex display exponentially faster as time goes on between the time remaining and emty screen when the time is up
    if(now - lastBlink > blinkDuration) 
    {
        lastBlink = now;

        // Calculate the ratio of elapsed time to total end screen duration 
        float ratio = (float)(now - startRoomTimeMillis) /(float)(DISPLAY_5S - FAST_BLINK_DURATION);
        if (ratio > 1.0f) ratio = 1.0f;
        blinkDuration = MAX_BLINK_DURATION_TIMEOUT - (MAX_BLINK_DURATION_TIMEOUT - MIN_BLINK_DURATION_TIMEOUT) * (ratio * ratio);

        buzzer_play(BUZZERT_DURATION);

        //hex display blinks between 0:0 and off
        if(displayStatus) hexDisplay_setTime(0, 0);
        else displayDigitsValues(OFF, OFF, OFF, OFF, OFF);
        displayStatus = !displayStatus;
        
    }
    //Waits until the time up template is done playing before sending event to go to next screen
    if(!isDisplayTemplateDonePlaying()) return;

    playAudio(AUDIO_TIME_IS_UP);

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
