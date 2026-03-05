#include "display_template.h"
#include "game_data.h"
#include <string.h>
#include "time_millis.h"
#include "buzzer.h"
#include <stdint.h>
#include <stdbool.h>
#include "touch_sensor.h"

#define MS_PER_TICK_PANALTY 10

uint32_t timeGamePanaltyBuffer=0;
uint32_t timeGamePenaltyMillis=0;
uint32_t timeRoomPanaltyMillis=0;
uint32_t startGameMillis=0;
uint8_t roomIndex=0;

uint32_t getWrongAnswerPenalty();
bool isInputMatching(const  char *input, const char *correctInput);
int32_t getTimeRemaining();

void setMapCoordinates(uint8_t coordinates[2])
{
    static uint8_t lastCoordinates[2] = {INVALID_COORD, INVALID_COORD};
    if(coordinates[0] == lastCoordinates[0] && coordinates[1] == lastCoordinates[1]) return; 
    
    lastCoordinates[0] = coordinates[0];
    lastCoordinates[1] = coordinates[1];


    if(coordinates[0] == INVALID_COORD || coordinates[1] == INVALID_COORD) 
    {
        return;
    }
}

compartment_t openCompartment(compartment_t compartment)
{
    switch (compartment)
    {
        case ONE_C:
 
            break;
            
        case TWO_C:

            break;

        default:
            break;
    }
    return NON_C;
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
    static uint32_t lastMinDisplayTime = 0;
    static uint32_t lastUpdateDisplayMillis = 0;
    static displayTemplate_t lastDisplayTemplate = RESET_D;



    uint32_t now = millis();
    if(forceDisplay && lastDisplayTemplate != displayTemplate)
    {
        #if DEBUG_ON_PC
        printf(displayTemplates[displayTemplate]);
        #endif
        lastDisplayTemplate = displayTemplate;
        lastUpdateDisplayMillis = now;
        lastMinDisplayTime = minDisplayTime;
    }

    if(now - lastUpdateDisplayMillis < lastMinDisplayTime) return false;

    lastUpdateDisplayMillis = now;
    lastMinDisplayTime = minDisplayTime;
    
    if(lastDisplayTemplate == displayTemplate) return true;

    #if DEBUG_ON_PC
    printf(displayTemplates[displayTemplate]);
    #endif

    lastDisplayTemplate = displayTemplate;


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
    static specialActies_t lastSpecialActies = NON_S;
    specialActies_t newAction = NON_S;

    if     (isTouchLongPressed()) newAction = TOUCH_SENSOR;
    else if(false) newAction = TWO_S;
    
    if(newAction != lastSpecialActies)
    { 
        lastSpecialActies = newAction;
        return newAction;
    }
    
    return NON_S;
}

/**
 * @brief Past de straf toe voor een fout antwoord.
 */
void applyWrongAnswerPenalty()
{
    runData.wrongAnswerCount++;

    uint32_t penalty = getWrongAnswerPenalty();
    timeRoomPanaltyMillis += penalty;
    timeGamePanaltyBuffer += penalty;
}

/**
 * @brief Returns the total elapsed game time in milliseconds.
 *
 * Calculates the time passed since the game started and adds any
 * accumulated penalty time.
 */
uint32_t getElapsedTime()
{
    return (millis() - startGameMillis) + timeGamePenaltyMillis; 
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
void updateTimeGamePanaltuMillis()
{
    if(timeGamePanaltyBuffer <= 0) return;
    if(timeGamePanaltyBuffer >= MS_PER_TICK_PANALTY)
    {
        timeGamePanaltyBuffer -= MS_PER_TICK_PANALTY;
        timeGamePenaltyMillis += MS_PER_TICK_PANALTY;
    }
    else
    {
        timeGamePenaltyMillis += timeGamePanaltyBuffer;
        timeGamePanaltyBuffer = 0;
    }
    
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
        
        if(isInputMatching(roomsSettings[roomIndex].answers[i], userInput)) return true;
    }
   
    return false;
}

/**
 * @brief Controleert of de gebruikersinvoer overeenkomt met het juiste antwoord.
 *
 * Vergelijkt twee en retourneert true als ze exact gelijk zijn.
 *
 * @param input         De string die gecontroleerd moet worden (bijvoorbeeld gebruikersinvoer).
 * @param correctInput  De string waarmee vergeleken wordt (bijvoorbeeld het juiste antwoord).
 *
 * @return true als de strings exact overeenkomen, anders false.
 */
bool isInputMatching(const  char *input, const char *correctInput)
{
    return (strcmp(correctInput, input) == 0);
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
    return  (elapsedTime  <= globalSettings.totalTime * 60.0f * 1000.0f) || (globalSettings.difficulty <= 2);
}

/**
 * @brief Updates and prints the remaining game time once per second.
 *
 */
void updateGameTimer()
{

  static int16_t lastSec =0;
  int32_t timeRemaining = getTimeRemaining();

  int32_t totalSec = timeRemaining / 1000;
  if(totalSec == lastSec) return;
  lastSec = totalSec;

  bool negative = false;
  if(totalSec < 0)
  {
      negative = true;
      totalSec = -totalSec;  // maak positief voor berekening
  }

  uint16_t minutes = totalSec / 60;
  uint16_t seconds = totalSec % 60;
    if(negative)
    {
        minutes = seconds;
        seconds  = minutes;
    }
    buzzer_play(BUZZERT_DURATION); // Zet buzzer aan als tijd negatief is, uit als tijd positief is
    #if DEBUG_ON_PC
        //  if(negative) printf("Time: -%02u:%02u\n",minutes, seconds);
        //  else printf("Time: %02u:%02u\n",minutes, seconds);
    #endif

}

/**
 * @brief Calculates the remaining game time in milliseconds.
 *
 * This function determines how much time is left before the global game
 * timer expires. The value can become negative when the elapsed time
 * exceeds the configured total game time.
 *
 * @return int32_t Remaining time in milliseconds.
 */
int32_t getTimeRemaining()
{
  uint32_t elapsedTime = getElapsedTime();
  int32_t timeRemaining = (globalSettings.totalTime * 60 * 1000) - elapsedTime;
  return timeRemaining;
}

/**
 * @brief Geeft het aantal geconfigureerde kamers terug.
 *
 *
 * @return uint8_t Het aantal gevonden kamers.
 */
uint8_t getNumRooms(void) 
{
    uint8_t count = 0;
    for(uint8_t i = 0; i < ARRAY_SIZE(roomsSettings); i++) {
        if(roomsSettings[i].beconIp[0] == '\0') break;
        count++;
    }
    return count;
}

/**
 * @brief Berekent de tijds-penalty voor een fout antwoord.
 * *
 * @return uint32_t De tijdsstraffen in milliseconden die moeten worden toegepast.
 */
uint32_t getWrongAnswerPenalty()
{
    uint32_t timePanalty = 0;
    switch (globalSettings.difficulty)
    {
        case WRONG_ANSWER_MINUS_1MIN_CONTINUE:
            timePanalty = 60 * 1000; // 1 minuut
            break;

        case WRONG_ANSWER_MINUS_5MIN_CONTINUE:
            timePanalty = 5 * 60 * 1000; // 5 minuten
            break;

        case WRONG_ANSWER_MINUS_5MIN_STOP:
            timePanalty = 5 * 60 * 1000; // 5 minuten, spel stopt bij 0
            break;

        case WRONG_ANSWER_MINUS_15MIN_STOP:
            timePanalty = 15 * 60 * 1000; // 15 minuten, spel stopt bij 0
            break;

        case WRONG_ANSWER_HALF_REMAINING_STOP:
        
            uint32_t elapsedTime = millis() - startGameMillis; 
            uint32_t remainingTime = (globalSettings.totalTime * 60 * 1000) - (elapsedTime + timeGamePenaltyMillis);
            timePanalty = remainingTime / 2;
            break;

        default:
            return 0;
            break;
    }
    return timePanalty;

}