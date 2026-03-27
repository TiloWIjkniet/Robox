
#include <string.h>
#include <fsm.h>
#include "game_logic.h"
#include "game_data.h"
#include <stdbool.h>
#include <stdint.h>
#include "time_millis.h"
#include "keypad.h"
#include "touch_sensor.h"
#include "HM10.h"

#define DISPLAY_5S 5000
#define DISPLAY_3S 3000

uint32_t startRoomMillis;
bool hasAnwertCorrect = false;
bool gameActiv = false;

typedef struct 
{
    displayTemplate_t waitForRoom;
    displayTemplate_t enteredRoom;
    displayTemplate_t enterAnswer;
    displayTemplate_t answerCorrect;
    displayTemplate_t answerWrong;
    displayTemplate_t specialActionPrompt;
    displayTemplate_t specialActionCorrect;
    displayTemplate_t specialActionWrong;
    displayTemplate_t openCompartment;


} roomDisplayConfig_t;

const roomDisplayConfig_t firstRoom =
{
    .waitForRoom = KAMER_D,
    .enteredRoom = KAMER_D,
    .enterAnswer = ANTWOORD_D,
    .answerCorrect = GOED_D,
    .answerWrong = FOUD_D,
    .specialActionPrompt = GOED_S_D,
    .specialActionCorrect = GOED_D,
    .specialActionWrong = FOUD_D,
    .openCompartment = ANTWOORD_D
};

const roomDisplayConfig_t middelRoom =
{
    .waitForRoom = KAMER_D,
    .enteredRoom = KAMER_D,
    .enterAnswer = ANTWOORD_D,
    .answerCorrect = GOED_D,
    .answerWrong = FOUD_D,
    .specialActionPrompt = GOED_S_D,
    .specialActionCorrect = GOED_D,
    .specialActionWrong = FOUD_D,
    .openCompartment = ANTWOORD_D
};

const roomDisplayConfig_t lastRoom =
{
    .waitForRoom = KAMER_D,
    .enteredRoom = KAMER_D,
    .enterAnswer = ANTWOORD_D,
    .answerCorrect = GOED_D,
    .answerWrong = FOUD_D,
    .specialActionPrompt = GOED_S_D,
    .specialActionCorrect = GOED_D,
    .specialActionWrong = FOUD_D,
    .openCompartment = ANTWOORD_D
};


typedef enum {STATE_WAIT_FOR_ROOM, STATE_ENTER_ANSWER, STATE_SPECIAL_ACTION, STATE_HANDLE_COMPARTMENT, STATE_FINISHED}room_stat_t;
room_stat_t state = STATE_WAIT_FOR_ROOM; 

void commonRoom_onEntry();
void commonRoom_onExit();
void commonRoom_Compartment(roomDisplayConfig_t roomDisplay);
bool commonRoom_SpesialAction(roomDisplayConfig_t roomDisplay);
bool commonRoom_AnswerCheck(roomDisplayConfig_t roomDisplay);
void commonRoom_onUpdate(roomDisplayConfig_t roomDisplay);

void first_room_onEntry(void)
{
    //Reset run data
    resetVirtualTime();

    gameActiv = true;
    roomIndex = 0;
    memset(&runData, 0, sizeof(runData));

    //Slaat wat eerste data op
    runData.totalTime  = globalSettings.totalTime;
    runData.difficulty = globalSettings.difficulty;
    runData.maxRooms   = getNumRooms();

    uint32_t now = millis();   

    startGameMillis = now; 
    timeGamePanaltyMillis = 0;

    commonRoom_onEntry();

    forceDisplayTemplate(START_D, DISPLAY_5S);
}
void first_room_onUpdate(void)
{
    commonRoom_onUpdate(firstRoom);
}
void first_room_onExit(void)
{
    commonRoom_onExit();
}

void room_loop_onEntry(void)
{
    roomIndex ++; 
    commonRoom_onEntry();
    forceDisplayTemplate(START_D, DISPLAY_5S);
}
void room_loop_onUpdate(void)
{ 
    commonRoom_onUpdate(middelRoom);
}
void room_loop_onExit(void)
{
    commonRoom_onExit();
}

void last_room_onEntry(void)
{   
    roomIndex ++; 
    commonRoom_onEntry();
    forceDisplayTemplate(START_D, DISPLAY_5S);
}
void last_room_onUpdate(void)
{
    commonRoom_onUpdate(lastRoom);
}
void last_room_onExit(void)
{
    commonRoom_onExit();
}

/**
 * @brief Common onEntry function for all room states.
 */
void commonRoom_onEntry()
{
    emptyInputBuffer();
    setMapCoordinates(roomsSettings[roomIndex].coordinates);

    startRoomMillis = startGameMillis; 
    hasAnwertCorrect = false;
    
    state = STATE_WAIT_FOR_ROOM;
}

/**
 * @brief Common onExit function for all room states.
 */
void commonRoom_onExit()
{
    uint32_t roomElapsedMillis = (startGameMillis - startRoomMillis);  
    
    float elapsedMinutes = ((float)roomElapsedMillis) / 1000.0f / 60.0f;
    runData.roomTimes[roomIndex] = elapsedMinutes;

    // uint32_t totalSec = roomElapsedMillis / 1000;   // alles naar seconden
    // uint16_t minutes = totalSec / 60;               // minuten
    // uint16_t seconds = totalSec % 60;               // resterende seconden


}

void commonRoom_Compartment(roomDisplayConfig_t roomDisplay)
{
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if(compartment != NON_C)
    {
        addDisplayTemplate(roomDisplay.openCompartment, DISPLAY_3S);
        openCompartment(compartment);
    }
}
bool commonRoom_SpesialAction(roomDisplayConfig_t roomDisplay)
{
    specialActies_t requiredSpeciaAction = roomsSettings[roomIndex].specialActies;
    if(requiredSpeciaAction == NON_S)
    {
        return true;
    }
    setRequiredSpecialActies(requiredSpeciaAction, true);
    addDisplayTemplate(roomDisplay.specialActionPrompt, DISPLAY_3S);
    specialActies_t preformedAction = getSpecialActies();
    if(preformedAction == NON_S) return false;
    
    if(preformedAction != requiredSpeciaAction)
    {
        forceDisplayTemplate(roomDisplay.specialActionWrong, DISPLAY_3S);
        applyWrongAnswerPenalty();
        return false;
    }
    forceDisplayTemplate(roomDisplay.specialActionCorrect, DISPLAY_3S);
    setRequiredSpecialActies(requiredSpeciaAction , false);
    return true;
    
}
bool commonRoom_AnswerCheck(roomDisplayConfig_t roomDisplay)
{
    updateInputBuffer();
    if(!hasNewAnswer) return false;
    bool correct = isAnswerCorrect(answerBuffer); 
    if(correct)
    {
        forceDisplayTemplate(roomDisplay.answerCorrect, DISPLAY_3S); 
        return true;
    }
    applyWrongAnswerPenalty();
    forceDisplayTemplate(roomDisplay.answerWrong, DISPLAY_3S); 
    return false;  
}
  
void commonRoom_onUpdate(roomDisplayConfig_t roomDisplay)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    switch (state)
    {
        case STATE_WAIT_FOR_ROOM:
        {
            bool inCorrectRoom = isInCorrectRoom(beconIp);
            addDisplayTemplate(roomDisplay.waitForRoom, 0);
            if(inCorrectRoom)
            {
                forceDisplayTemplate(roomDisplay.enteredRoom, DISPLAY_3S);
                state = STATE_ENTER_ANSWER;
            }
            break;
        }
        case STATE_ENTER_ANSWER:
        {
            addDisplayTemplate(roomDisplay.enterAnswer, 0);
            bool result = commonRoom_AnswerCheck(roomDisplay);
            if     (result) state = STATE_SPECIAL_ACTION;
            break;
        }
        case STATE_SPECIAL_ACTION:
        {
            bool preformedAction =commonRoom_SpesialAction(roomDisplay);
            if(preformedAction) state = STATE_HANDLE_COMPARTMENT;
            break;
        }
        case STATE_HANDLE_COMPARTMENT:
        {
            commonRoom_Compartment(roomDisplay);
            state = STATE_FINISHED;
            break;
        }
        case STATE_FINISHED:
        {
            if(isDisplayTemplateDonPlaying())
            {
                if(roomIndex < getNumRooms() - 2)       FSM_addEvent(E_ROOM_COMPLETED);
                else if (roomIndex < getNumRooms() - 1) FSM_addEvent(E_ROOM_LOOP_TO_LAST);
                else                                    FSM_addEvent(E_LAST_ROOM_COMPLETED);
            }
 
            break;
        }
        default:
        {
            break;
        }
    }
}