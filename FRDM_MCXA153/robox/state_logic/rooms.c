
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
#include "audio.h"

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
    displayTemplate_t specialActionCorrect;
    displayTemplate_t specialActionWrong;
    displayTemplate_t openCompartment;


} roomDisplayConfig_t;

const roomDisplayConfig_t firstRoom =
{
    .waitForRoom = D_GO_TO_ROOM_3,
    .enteredRoom = D_IN_ROOM,
    .enterAnswer = D_ENTER_ANSWER,
    .answerCorrect = D_FIRST_SUCCESS,
    .answerWrong = D_WRONG_1,
    .specialActionCorrect = D_SPECIAL_CORRECT  ,
    .specialActionWrong = D_SPECIAL_WRONG,
    .openCompartment = D_OPEN_COMPARTMENT
};

const roomDisplayConfig_t middelRoom =
{
    .waitForRoom = D_GO_TO_ROOM_2,
    .enteredRoom = D_IN_ROOM,
    .enterAnswer = D_ENTER_ANSWER,
    .answerCorrect = D_CORRECT_2,
    .answerWrong = D_WRONG_2,
    .specialActionCorrect = D_SPECIAL_CORRECT,
    .specialActionWrong = D_SPECIAL_WRONG,
    .openCompartment = D_OPEN_COMPARTMENT
};

const roomDisplayConfig_t lastRoom =
{
    .waitForRoom = D_GO_TO_FINAL_ROOM,
    .enteredRoom = D_FINAL_ROOM_ENTER,
    .enterAnswer = D_ENTER_ANSWER_STRESS,
    .answerCorrect = D_CORRECT_3,
    .answerWrong = D_WRONG_3,
    .specialActionCorrect = D_SPECIAL_CORRECT,
    .specialActionWrong = D_SPECIAL_WRONG,
    .openCompartment = D_OPEN_COMPARTMENT
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

    forceDisplayTemplate(D_START_GAME, DISPLAY_5S);

    playAudio(BOM_HAS_BEEN_PLANTED);
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
    forceDisplayTemplate(D_START_GAME, DISPLAY_5S);
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
    forceDisplayTemplate(D_START_GAME, DISPLAY_5S);
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

    displayTemplate_t specialActieTemplate;
    if(requiredSpeciaAction == TOUCH_SENSOR) specialActieTemplate = D_SCAN; 
    else if(requiredSpeciaAction == NON_S) specialActieTemplate = D_KEY; 
    else if(requiredSpeciaAction == NON_S) specialActieTemplate = D_SWITCH; 

    addDisplayTemplate(specialActieTemplate, DISPLAY_3S);
    specialActies_t preformedAction = getSpecialActies();
    if(preformedAction == NON_S) return false;
    
    if(preformedAction != requiredSpeciaAction)
    {
        forceDisplayTemplate(roomDisplay.specialActionWrong, DISPLAY_3S);
        applyWrongAnswerPenalty();
        playGlobelAudio(WRONG_ANSWER);
        return false;
    }
    forceDisplayTemplate(roomDisplay.specialActionCorrect, DISPLAY_3S);
    setRequiredSpecialActies(requiredSpeciaAction , false);
    playGlobelAudio(CORRECT_ANSWER);

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
        playGlobelAudio(CORRECT_ANSWER);
        return true;
    }
    applyWrongAnswerPenalty();
    forceDisplayTemplate(roomDisplay.answerWrong, DISPLAY_3S); 
    playGlobelAudio(WRONG_ANSWER);
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
                playGlobelAudio(CORRECT_ANSWER);
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
            if(preformedAction)
            {
                state = STATE_HANDLE_COMPARTMENT;
            }
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