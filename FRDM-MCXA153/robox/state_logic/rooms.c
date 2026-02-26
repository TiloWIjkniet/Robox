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
    NON,
    ONE,
    TWO
}compartment_t;
typedef enum
{
    NON,
    ONE,
    TWO
}specialActies_t;

typedef enum
{
    NON,
    ONE,
    TWO
}displayTemplate_t;

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
char displayTemplates[10][MAX_CHAR_IN_STRING];

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

    uint16_t now = 0/*millis()*/;   // naar milis();
    startRoomMillis = now; 
    startGameMillis = now; 

    timeRoomPanaltyMillis = 0;
    timeGamePenaltyMillis = 0;

    hasAnwertCorrect = false;

    //setMapCoordinates(roomsSettings[roomIndex].coordinates) // moet nog
    displayLoadTemplate(NON,0 , true); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
}
void first_room_onUpdate(void)
{
    if(!isWithinTimeLimit()) {FSM_addEvent(E_ROOM_TIMEOUT); return;}

    bool isInCorrectRoom = isInCorrectRoom("BECON IP VAN DICHTSBIJZIJNDE BEACON");
    if(!isInCorrectRoom) return;


    if(!hasAnwertCorrect)
    {
        displayLoadTemplate(NON, 0, false); // TEMPLATE MOET NOG GEFULT WORDEN MET TEXT
        if(!hasNewAnswer) return;
        bool correct = isAnswerCorrect("ANTWOORD UIT BUFFER"); 

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

    if(displayFinished) FSM_addEvent(E_ROOM_COMPLETED);
}
void first_room_onExit(void)
{
    uint32_t roomElapsedMillis = (0/*millis()*/ - startRoomMillis) + timeRoomPanaltyMillis   //millis() // moet nog
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

void openCompartment(compartment_t compartment)
{
    switch (compartment)
    {
        case ONE:
            #warning HIER MOET NOG LOGICA
            break;
            
        case TWO:
            #warning HIER MOET NOG LOGICA
            break;

        default:
            break;
    }
}

/**
 * @brief Laadt en toont een displaytemplate op het scherm.
 *
 * Deze functie zorgt ervoor dat een displaytemplate alleen wordt
 * weergegeven als:
 * - Het template anders is dan het laatst getoonde template.
 * - De minimale weergavetijd van het vorige template verstreken is.
 *
 * @param displayTemplate   Het template dat weergegeven moet worden.
 * @param minDisplayTime    Minimale tijd (in milliseconden) dat het
 *                          huidige template zichtbaar moet blijven
 *                          voordat een nieuw template geladen mag worden.
 * @return                  True als het template succesvol is geladen anders false.

 */
bool displayLoadTemplate(displayTemplate_t displayTemplate, uint32_t minDisplayTime, bool forceDisplay)
{
    static uint32_t lastUpdateDisplayMillis = 0;
    static displayTemplate_t lastDisplayTemplate = NON;

    uint32_t now = 0/*millis()*/; //moet nog;
    if(now - lastUpdateDisplayMillis < minDisplayTime) return false;
    if(lastDisplayTemplate == displayTemplate) return true;

    lastUpdateDisplayMillis = now;
    lastDisplayTemplate = displayTemplate;
    printf(displayTemplates[displayTemplate]); // moet nog
    return true;
}

/**
 * @brief Bepaalt welke speciale actie uitgevoerd moet worden.
 *
 * Deze functie controleert of er een speciale actie actief is en
 * voorkomt dat dezelfde actie meerdere keren achter elkaar wordt
 * teruggegeven. 
 * 
 * @return NON  Als er geen nieuwe speciale actie is.
 * 
 * @warning Moet nog logica toe gevoegt worden
 */
specialActies_t getSpecialActies()
{
    static specialActies_t lastSpecialActies = NON;
    specialActies_t newAction =NON;

    if     (true) newAction = ONE;
    else if(true) newAction = TWO;
    
    if(newAction != lastSpecialActies) return newAction;
    
    return NON;
}

/**
 * @brief Past de straf toe voor een fout antwoord.
 */
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
 * @brief Controleert of de speler zich in de juiste kamer bevindt.
 *
 * Vergelijkt het opgegeven beacon-IP adres met het beacon-IP adres
 * van de huidige kamer (roomsSettings[roomIndex]).
 *
 * @param beconIp  Het beacon-IP adres dat gedetecteerd is.
 * @return true    Als het beacon-IP overeenkomt met de huidige kamer.
 * @return false   Als het beacon-IP niet overeenkomt.
 */
bool isInCorrectRoom(char *beconIp)
{
    if(strcmp(roomsSettings[roomIndex].beconIp, beconIp) == 0) return true;
    return false;
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