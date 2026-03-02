#include <stdint.h>
#include <string.h>
#include <fsm.h>
#include "game_logic.h"
#include "game_data.h"

//Debug options 


uint32_t timeRoomPanaltyMillis;
uint32_t startRoomMillis;

bool hasAnwertCorrect = false;
uint8_t roomIndex;

void first_room_onEntry(void)
{
    //Reset run data
    roomIndex = 0;
    memset(&runData, 0, sizeof(runData));

    //Slaat wat eerste data op
    runData.totalTime  = globalSettings.totalTime;
    runData.difficulty = globalSettings.difficulty;
    runData.maxRooms   = ARRAY_SIZE(roomsSettings);

    uint32_t now = 0/*millis()*/;   // naar milis();

    startGameMillis = now; 
    timeGamePenaltyMillis = 0;

    commonRoom_onEntry();

   
    displayLoadTemplate(NON,0 , true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
}
void first_room_onUpdate(void)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    bool isInCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");
    if(!isInCorrectRoom) 
    {
        displayLoadTemplate(NON,0 , false);
        hasNewAnswer = false;
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(NON, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON)
    {
        //Spechale actie nodig
        displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON) return;
        if(performed != required)
        {
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    bool displayFinished = false;
    if (compartment != NON)
    {
        displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        displayFinished = openCompartment(compartment);
    }
    else
    {
        displayFinished = displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }

    if(!displayFinished) return;

    FSM_addEvent(E_ROOM_COMPLETED);
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
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    bool isInCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");
    if(!isInCorrectRoom) 
    {
        displayLoadTemplate(NON,0 , false);
        hasNewAnswer = false;
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(NON, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON)
    {
        //Spechale actie nodig
        displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON) return;
        if(performed != required)
        {
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    bool displayFinished = false;
    if (compartment != NON)
    {
        displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        displayFinished = openCompartment(compartment);
    }
    else
    {
        displayFinished = displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }

    if(!displayFinished) return;
    if(roomIndex < ARRAY_SIZE(roomsSettings) - 1) FSM_addEvent(E_ROOM_COMPLETED);
    else FSM_addEvent(E_ROOM_LOOP_TO_LAST);   
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
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    bool isInCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");
    if(!isInCorrectRoom) 
    {
        displayLoadTemplate(NON,0 , false);
        hasNewAnswer = false;
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(NON, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON)
    {
        //Spechale actie nodig
        displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON) return;
        if(performed != required)
        {
            displayLoadTemplate(NON,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    bool displayFinished = false;
    if (compartment != NON)
    {
        displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        displayFinished = openCompartment(compartment);
    }
    else
    {
        displayFinished = displayLoadTemplate(NON, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }

    if(!displayFinished) return;
    FSM_addEvent(E_LAST_ROOM_COMPLETED);

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
    setMapCoordinates(roomsSettings[roomIndex].coordinates);

    uint16_t now = 0/*millis()*/;   // naar milis();
    startRoomMillis = now; 
    timeRoomPanaltyMillis = 0;
    hasAnwertCorrect = false;
    hasNewAnswer = false;
}

/**
 * @brief Common onExit function for all room states.
 */
void commonRoom_onExit()
{
    uint32_t roomElapsedMillis = (0/*millis()*/ - startRoomMillis) + timeRoomPanaltyMillis   //millis() // moet nog
    runData.roomTimes[roomIndex] = (uint32_t)(roomElapsedMillis / 1000); // millis naar sec
}



