
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

uint32_t startRoomMillis;
bool hasAnwertCorrect = false;
bool gameActiv = false;


void commonRoom_onEntry();
void commonRoom_onExit();

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
    timeGamePenaltyMillis = 0;

    commonRoom_onEntry();

    addDisplayTemplate(START_D, 5 * 1000);
}
void first_room_onUpdate(void)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}
    bool inCorrectRoom = isInCorrectRoom(beconIp);

    if(!inCorrectRoom) 
    {
        addDisplayTemplate(KAMER_D, 0);
        return;
    }


    if(!hasAnwertCorrect)
    {
        addDisplayTemplate(ANTWOORD_D, 0);

        updateInputBuffer(); // meschein ergens anders neer zetten dat je ook als je neit in de kamer bent antwoorden kan doen
        
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            addDisplayTemplate(FOUD_D, 3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET zTEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        serRequiredSpecialActies(required, true);
        addDisplayTemplate(GOED_S_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            forceDisplayTemplate(FOUD_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
        serRequiredSpecialActies(required, false);
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if (compartment != NON_C)
    {
        forceDisplayTemplate(GOED_C_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        forceDisplayTemplate(GOED_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
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

    bool inCorrectRoom = isInCorrectRoom(beconIp);

    if(!inCorrectRoom) 
    {
        addDisplayTemplate(KAMER_D, 0);
        return;
    }


    if(!hasAnwertCorrect)
    {
        addDisplayTemplate(ANTWOORD_D, 0); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT

        updateInputBuffer();
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            forceDisplayTemplate(FOUD_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        serRequiredSpecialActies(required, true);
        addDisplayTemplate(GOED_S_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            forceDisplayTemplate(FOUD_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
        serRequiredSpecialActies(required, false);
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;

    if (compartment != NON_C)
    {
        forceDisplayTemplate(GOED_C_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        forceDisplayTemplate(GOED_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
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

    bool inCorrectRoom = isInCorrectRoom(beconIp);



    if(!inCorrectRoom) 
    {
        addDisplayTemplate(KAMER_D,0);
        return;
    }


    if(!hasAnwertCorrect)
    {
        addDisplayTemplate(ANTWOORD_D, 0);

        updateInputBuffer();
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect(answerBuffer); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            forceDisplayTemplate(FOUD_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON_S)
    {
        serRequiredSpecialActies(required, true);
        addDisplayTemplate(GOED_S_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        specialActies_t performed = getSpecialActies();
        if(performed == NON_S) return;
        if(performed != required)
        {
            forceDisplayTemplate(FOUD_D,3 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
            applyWrongAnswerPenalty();
            return; 
        } 
        serRequiredSpecialActies(required, false);
    }

    //Alles goed ga door
    compartment_t compartment = roomsSettings[roomIndex].openCompartment;
    if (compartment != NON_C)
    {
        forceDisplayTemplate(GOED_C_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        openCompartment(compartment);
    }
    else
    {
        forceDisplayTemplate(GOED_D, 5 * 1000); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
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
    setMapCoordinates(roomsSettings[roomIndex].coordinates);

    startRoomMillis = startGameMillis; 
    hasAnwertCorrect = false;
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



  