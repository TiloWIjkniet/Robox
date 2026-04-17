
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



uint32_t startRoomMillis;
bool hasAnwertCorrect = false;
bool isGameActiv = false;

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

void commonRoom_onEntry(void);
void commonRoom_onExit(void);
void commonRoom_Compartment(roomDisplayConfig_t roomDisplay);
bool commonRoom_SpesialAction(roomDisplayConfig_t roomDisplay);
bool commonRoom_AnswerCheck(roomDisplayConfig_t roomDisplay);
void commonRoom_onUpdate(roomDisplayConfig_t roomDisplay);

void first_room_onEntry(void)
{
    //Reset run data

    resetGameLogic();

    isGameActiv = true;
    roomIndex = 0;

    //leegt run data 
    memset(&runData, 0, sizeof(runData));

    //Saves global settings to run data
    runData.totalTime  = globalSettings.totalTime;
    runData.difficulty = globalSettings.difficulty;
    runData.maxRooms   = getNumRooms();

    //Start timer
    uint32_t now = millis();   
    startGameMillis = now; 
    timeGamePanaltyMillis = 0;
    timeGamePanaltyBuffer = 0;

    commonRoom_onEntry();

    forceDisplayTemplate(D_START_GAME, DISPLAY_5S);

    playAudio(AUDIO_BOM_HAS_BEEN_PLANTED);
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
void commonRoom_onEntry(void)
{
    emptyInputBuffer();
    setMapCoordinates(roomsSettings[roomIndex].coordinates);

    //Reset room specific variables
    startRoomMillis = startGameMillis; 
    hasAnwertCorrect = false;
    state = STATE_WAIT_FOR_ROOM;
}

/**
 * @brief Common onExit function for all room states.
 */
void commonRoom_onExit(void)
{
    //Update run data with room time
    uint32_t roomElapsedMillis = (startGameMillis - startRoomMillis);  
    float elapsedMinutes = ((float)roomElapsedMillis) / (float)(1 * FROM_MIN_TO_MS);
    runData.roomTimes[roomIndex] = elapsedMinutes;
}

void commonRoom_Compartment(roomDisplayConfig_t roomDisplay)
{
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if(compartment != NON_C)
    {
        addDisplayTemplate(roomDisplay.openCompartment, DISPLAY_3S);
        openCompartment(compartment);
        playAudio(AUDIO_OPEN_COMPARTMENT);
    }
}
bool commonRoom_SpesialAction(roomDisplayConfig_t roomDisplay)
{
    specialActies_t requiredSpeciaAction = roomsSettings[roomIndex].specialActies;
    if(requiredSpeciaAction == NON_S)
    {
        //Skip special action if not required
        return true;
    }
    setRequiredSpecialActies(requiredSpeciaAction, true);

    //Set display template based on required special actie
    displayTemplate_t specialActieTemplate;
    if(requiredSpeciaAction == FINGER_PRINT_S)  specialActieTemplate = D_SCAN; 
    else if(requiredSpeciaAction == KEY_S)      specialActieTemplate = D_KEY; 
    else if(requiredSpeciaAction == SWITCH_S)   specialActieTemplate = D_SWITCH; 
    addDisplayTemplate(specialActieTemplate, DISPLAY_3S);

    //handle special actie input and check if correct
    specialActies_t preformedAction = getSpecialActies();
    if(preformedAction == NON_S) return false;
    //If a special actie is preformed
    if(preformedAction != requiredSpeciaAction)
    {
        //if wrong special actie preformed
        //NOTE: Time panelty for wrong special actie is handelt in game logic updateSpecialActies() 
        forceDisplayTemplate(roomDisplay.specialActionWrong, DISPLAY_3S);
        return false;
    }
    //If correct special actie preformed
    forceDisplayTemplate(roomDisplay.specialActionCorrect, DISPLAY_3S);
    setRequiredSpecialActies(requiredSpeciaAction , false);
    

    return true;
}
bool commonRoom_AnswerCheck(roomDisplayConfig_t roomDisplay)
{
    
    if(!hasNewAnswer) return false;
    //New answer is in answer buffer
    bool correct = isAnswerCorrect(answerBuffer); 
    if(correct)
    {
        //If answer is correct
        forceDisplayTemplate(roomDisplay.answerCorrect, DISPLAY_3S); 
        playGlobelAudio(AUDIO_CORRECT_ANSWER);
        return true;
    }
    //if answer is wrong
    applyWrongAnswerPenalty();
    forceDisplayTemplate(roomDisplay.answerWrong, DISPLAY_3S); 
    playGlobelAudio(AUDIO_WRONG_ANSWER);
    return false;  
}
  
void commonRoom_onUpdate(roomDisplayConfig_t roomDisplay)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    switch (state)
    {
        case STATE_WAIT_FOR_ROOM: //Wait for player to enter the correct room
        {
            bool inCorrectRoom = isInCorrectRoom(beconIp);

            //TEMP: voor debug options/ voor def mode 
            if(hasNewAnswer) {inCorrectRoom = inCorrectRoom || isInputMatching(answerBuffer, "0000");}
            addDisplayTemplate(roomDisplay.waitForRoom, 0);
            if(inCorrectRoom)
            {
                //If in correct room go to next state and set display template and play audio
                forceDisplayTemplate(roomDisplay.enteredRoom, DISPLAY_3S);
                playGlobelAudio(AUDIO_CORRECT_ROOM);
                state = STATE_ENTER_ANSWER;
            }
            break;
        }
        case STATE_ENTER_ANSWER: //Wait for player to enter correct answer
        {
            addDisplayTemplate(roomDisplay.enterAnswer, 0);
            bool result = commonRoom_AnswerCheck(roomDisplay);
            if     (result) state = STATE_SPECIAL_ACTION; //If answer correct go to next state
            break;
        }
        case STATE_SPECIAL_ACTION: //Wait for player to perform special action if required
        {
            bool preformedAction = commonRoom_SpesialAction(roomDisplay);
            if(preformedAction) state = STATE_HANDLE_COMPARTMENT; //If special action correct go to next state
            break;
        }
        case STATE_HANDLE_COMPARTMENT: //Handle compartment opening
        {
            commonRoom_Compartment(roomDisplay);
            state = STATE_FINISHED; //Go to finished state
            break;
        }
        case STATE_FINISHED: //Wait for display template to finish before sending event to go to next room
        {
            if(isDisplayTemplateDonePlaying())
            {
                //If the finished template is done playing, got to the corect room based on how many rooms are in the game and how many rooms have been completed
                if(roomIndex < getNumRooms() - 2)       FSM_addEvent(E_ROOM_COMPLETED);// - 2 because room index is numRooms -1 and we want the second last room so -2
                else if (roomIndex < getNumRooms() - 1) FSM_addEvent(E_ROOM_LOOP_TO_LAST); // - 1 because room index is numRooms -1
                else                                    FSM_addEvent(E_LAST_ROOM_COMPLETED);
            }
 
            break;
        }
    }
}