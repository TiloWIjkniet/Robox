
#include <string.h>
#include <fsm.h>
#include "game_logic.h"
#include "game_data.h"
#include <stdbool.h>
#include <stdint.h>
#include "time_millis.h"
#include "keypad.h"
#include "touch_sensor.h"

uint32_t startRoomMillis;

bool hasAnwertCorrect = false;
bool gameActiv = false;


void commonRoom_onEntry();
void commonRoom_onExit();

void first_room_onEntry(void)
{
    //Reset run data
    gameActiv = true;
    roomIndex = 0;
    memset(&runData, 0, sizeof(runData));

    //Slaat wat eerste data op
    runData.totalTime  = globalSettings.totalTime;
    runData.difficulty = globalSettings.difficulty;
    runData.maxRooms   = getNumRooms();

    uint32_t now = millis();   

    startGameMillis = now; 
    timeGamePenaltyMillis = 0;

    commonRoom_onEntry();

    displayLoadTemplate(START_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
}
void first_room_onUpdate(void)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}
    bool inCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");


    if(!inCorrectRoom) 
    {
        displayLoadTemplate(KAMER_D,0 , false);
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(ANTWOORD_D, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT

        updateInputBuffer();
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET zTEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        //Spechale actie nodig
        displayLoadTemplate(GOED_S_D,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if (compartment != NON_C)
    {
       displayLoadTemplate(GOED_C_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        displayLoadTemplate(GOED_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }

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

    bool inCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");



    if(!inCorrectRoom) 
    {
        displayLoadTemplate(KAMER_D,0 , false);
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(ANTWOORD_D, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT

        updateInputBuffer();
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        //Spechale actie nodig
        displayLoadTemplate(GOED_S_D,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;

    if (compartment != NON_C)
    {
        displayLoadTemplate(GOED_C_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        displayLoadTemplate(GOED_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }



    if(roomIndex < getNumRooms() - 2) FSM_addEvent(E_ROOM_COMPLETED);
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

    bool inCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");



    if(!inCorrectRoom) 
    {
        displayLoadTemplate(KAMER_D,0 , false);
        return;
    }


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(ANTWOORD_D, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT

        updateInputBuffer();
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        //Spechale actie nodig
        displayLoadTemplate(GOED_S_D,3 * 1000, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            displayLoadTemplate(FOUD_D,3 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return; 
        } 
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if (compartment != NON_C)
    {
        displayLoadTemplate(GOED_C_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        displayLoadTemplate(GOED_D, 5 * 1000, true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
    }

  
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
    emptyInputBuffer();

    #if DEBUG_ON_PC
        printf("In room: %d\n", roomIndex);
    #endif

    setMapCoordinates(roomsSettings[roomIndex].coordinates);

    uint16_t now = millis();   
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
    uint32_t roomElapsedMillis = (millis() - startRoomMillis) + timeRoomPanaltyMillis;  
    
    float elapsedMinutes = ((float)roomElapsedMillis) / 1000.0f / 60.0f;
    runData.roomTimes[roomIndex] = elapsedMinutes;

    uint32_t totalSec = roomElapsedMillis / 1000;   // alles naar seconden
    uint16_t minutes = totalSec / 60;               // minuten
    uint16_t seconds = totalSec % 60;               // resterende seconden


    #if DEBUG_ON_PC
        printf("room %d: %02u:%02u\n", roomIndex, minutes, seconds);
    #endif

}



  