#include <stdint.h>
#include <string.h>
#include <fsm.h>
#define MAX_RUNS 20
#define MAX_ROOMS 20
#define MAX_CHAR_IN_STRING 50
#define MAX_ANSWERS 5
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef struct 
{
    uint16_t roomTimes[MAX_ROOMS];
    bool finished;
    uint8_t wrongAnswerCount;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;
}runData_t;

typedef enum
{
    NON
}compartment_t;
typedef enum
{
    NON
}specialActies_t;

typedef struct 
{
    int coordinates[2];
    char beconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING];
    compartment_t openCompartment;
    specialActies_t specialActies;
}roomSettings_t;

typedef struct 
{
    uint8_t difficulty;
    uint8_t totalTime;
    bool audio;

}globalSettings_t;

typedef enum
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,     // 1 minuut aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,     // 5 minuten aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,   // Tijd aftrekken (bijv. 5 min), stop als tijd 0
    WRONG_ANSWER_HALF_REMAINING_STOP 
}wrongAnswerPenalty_t

globalSettings_t globalSettings;
runData_t runData;
roomSettings_t roomsSettings[MAX_ROOMS];

uint32_t timeGamePanaltyBuffer;
uint32_t timeRoomPanaltyMillis;
uint32_t timeGamePenaltyMillis;
uint32_t startRoomMillis;
uint32_t startGameMillis;
uint8_t roomIndex;

bool hasNewAnswer = false;
bool hasAnwertCorrect = false;

void first_room_onEntry(void)
{
    //Reset run data
    roomIndex = 0;
    memset(&runData, 0, sizeof(runData));

    //Slaat wat eerste data op
    runData.totalTime  = globalSettings.totalTime;
    runData.difficulty = globalSettings.difficulty;
    runData.maxRooms   = ARRAY_SIZE(roomsSettings);

    uint16_t now = 0;   // naar milis();
    startRoomMillis = now; 
    startGameMillis = now; 

    timeRoomPanaltyMillis = 0;
    timeGamePenaltyMillis = 0;

    hasAnwertCorrect = false;

    //setMapCoordinates(roomsSettings[roomIndex].coordinates) // moet nog
    //Display Nog niet op goede location // moet nog
}
void first_room_onUpdate(void)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    bool isInCorrectRoom = true; // isInCorrectRoom(roomsSettings.beconIp) //Moet nog
    if(!isInCorrectRoom) return;
    //Display user input & luister naar user input  // moet nog

    if(!hasAnwertCorrect)
    {

        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect("ANTWOORD UIT BUFFER"); 

        if(!correct)
        {
            //Fout antwoord
            applyWrongAnswerPenalty();
            return;  
        }
        hasAnwertCorrect = true;
    }
    //Corect antwoord
    
    specialActies_t required = roomsSettings[roomIndex].specialActies;
    if(required != NON)
    {
        //Spechale actie nodig
        specialActies_t performed = getSpecialActies();
        if(performed == NON) return;
        if(performed != required)
        {
            applyWrongAnswerPenalty();
            return; 
        } 
    }

    //Alles goed ga door
    if (roomsSettings[roomIndex].openCompartment != NON)
    {
        // openCompartment(roomsSettings[roomIndex].openCompartment)
    }

    FSM_addEvent(E_ROOM_COMPLETED);
}
void first_room_onExit(void)
{
    uint32_t roomElapsedMillis = (0 - startRoomMillis) + timeRoomPanaltyMillis   //millis() // moet nog
    runData.roomTimes[roomIndex] = (uint32_t)(roomElapsedMillis / 1000);
}

void room_loop_onEntry(void)
{

}
void room_loop_onUpdate(void)
{
    
}
void room_loop_onExit(void)
{

}

void last_room_onEntry(void)
{   

}
void last_room_onUpdate(void)
{
   

}
void last_room_onExit(void)
{

}

//Moet nog gemaakt worden
specialActies_t getSpecialActies()
{
    return NON;
}

static inline void applyWrongAnswerPenalty(void)
{
    runData.wrongAnswerCount++;

    uint32_t penalty = getWrongAnswerPenalty();
    timeRoomPenaltyMillis += penalty;
    timeGamePenaltyBuffer += penalty;
}

/**
 * @brief Returns the total elapsed game time in milliseconds.
 *
 * Calculates the time passed since the game started and adds any
 * accumulated penalty time.
 */
inline uint32_t getElapsedTime()
{
    return (0 - startGameMillis) + timeGamePenaltyMillis; //millis
}

/**
 * @brief Updates the accumulated game time penalty in milliseconds.
 *
 * This function increments the total game penalty time by 1 millisecond
 * if there is still penalty time remaining in the penalty buffer.
 *
 * The function does nothing when the penalty buffer is empty or zero.
 *
 * @note Intended to be called in the main loop
 */
inline void updateTimeGamePanaltuMillis()
{
    if(timeGamePanaltyBuffer <= 0) return;
    timeGamePanaltyBuffer--;
    timeGamePenaltyMillis++;
    //UpdaeTimeDisplay();
}

/**
 * @brief Controleert of het opgegeven antwoord overeenkomt met één van de juiste antwoorden van de huidige kamer.
 *
 * @param userInput Pointer naar de gebruikersinvoer (C-string) die gecontroleerd moet worden.
 * @return true Als `userInput` overeenkomt met één van de juiste antwoorden.
 * @return false Als `userInput` niet overeenkomt met een van de antwoorden.
 */
bool isAnswerCorrect(char *userInput)
{
    hasNewAnswer  = false;
    for(int i = 0; i < MAX_ANSWERS; i++)
    {
        if(strcmp(roomsSettings[roomIndex].answers[i], userInput) == 0) return true;
    }
    return false;
}

/**
 * @brief Checks whether the game is still within the allowed time limit.
 *
 * If the difficulty level is 2 or lower, the time limit is ignored
 * and the function will always return true.
 *
 * @return true  If there is still time remaining OR time limit is disabled.
 * @return false If the time limit has been exceeded.
 */
bool isWithinTimeLimit(void)
{
    uint32_t elapsedTime = getElapsedTime(); 
    return  (elapsedTime  <= globalSettings.totalTime) || (globalSettings.difficulty <= 2);
}

/**
 * @brief Berekent de tijds-penalty voor een fout antwoord.
 * *
 * @return uint32_t De tijdsstraffen in milliseconden die moeten worden toegepast.
 */
uint32_t getWrongAnswerPenalty()
{
    switch (globalSettings.difficulty)
    {
        case WRONG_ANSWER_MINUS_1MIN_CONTINUE:
            return 60 * 1000; // 1 minuut
            break;

        case WRONG_ANSWER_MINUS_5MIN_CONTINUE:
            return 5 * 60 * 1000; // 5 minuten
            break;

        case WRONG_ANSWER_MINUS_5MIN_STOP:
            return 5 * 60 * 1000; // 5 minuten, spel stopt bij 0
            break;

        case WRONG_ANSWER_MINUS_15MIN_STOP:
            return 15 * 60 * 1000; // 15 minuten, spel stopt bij 0
            break;

        case WRONG_ANSWER_HALF_REMAINING_STOP:
        
            uint32_t elapsedTime = 0 - startGameMillis; // milliseconden sinds start
            uint32_t remainingTime = (globalSettings.totalTime) - (elapsedTime + timePenaltyMillis);
            return remainingTime / 2;
            break;

        default:
            return 0;
            break;
    }
}