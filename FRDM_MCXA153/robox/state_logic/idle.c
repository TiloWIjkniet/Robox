#include "game_data.h"
#include "display_template.h"
#include <fsm.h>
#include "game_logic.h"
#include <stdbool.h>
#include <stdint.h>
#include "keypad.h"
#include <stdio.h>
#include "hexDisplay.h"
#include "switch_and_key_sensors.h"
#include "audio.h"
#include "lock.h"

#define ENTER_DEV_CODE "0000"

void idle_onEntry(void)
{
    emptyInputBuffer();
    setMapCoordinates(NO_COORDINATES);
    
    displayDigits(IDLE_MODE_HEX);
}
void idle_onUpdate(void)
{
    addDisplayTemplate(D_IDLE, 0);

    if(!hasNewAnswer) return;

    if(isInputMatching(answerBuffer, ENTER_DEV_CODE)) {FSM_addEvent(E_START_DEV); return;} 
    
    //Zorgt ervoor dat als de switch of key nog niet gereset is, het spel niet start en er een foutmelding komt te staan.
    bool rawKeyState = readRawKeySensor();
    bool rawSwitchState = readRawSwitchSensor();
    bool lockStatus = isLockOpen();

    if(rawKeyState || rawSwitchState || lockStatus) 
    {
        displayDigits(ERROR_HEX);
        playGlobelAudio(AUDIO_ERROR);
        forceDisplayTemplate(D_ERROR_SWITCH_OR_KEY_NEETST_TO_BE_RESET, DISPLAY_5S);
        return;
    }
    
    FSM_addEvent(E_START_GAME); 
}
void idle_onExit(void)
{

}