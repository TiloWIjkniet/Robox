#include "display_template.h"
#include "game_data.h"
#include <string.h>
#include "time_millis.h"
#include "buzzer.h"
#include <stdint.h>
#include <stdbool.h>
#include "touch_sensor.h"
#include "hexDisplay.h"
#include "game_logic.h"
#include "audio.h"
#include "switch_and_key_sensors.h"
#define MS_PER_TICK_PANALTY 10
#define FROM_MIN_TO_MS 60 *1000
uint32_t timeGamePanaltyBuffer=0;
uint32_t timeGamePanaltyMillis=0;
uint32_t startGameMillis=0;
uint8_t roomIndex=0;

uint8_t virtualTimeMultiplier = 1;
uint32_t virtualTime = 0;
uint32_t lastRealTime = 0;

specialActies_t requiredSpecialActies = NON_S;
specialActies_t preformtSpecialAction = NON_S;
uint8_t requiredActions = 0;
uint16_t playedAudio = 0;

const uint8_t NO_COORDINATES[2] = {INVALID_COORD, INVALID_COORD};

uint32_t getWrongAnswerPenalty();
bool isInputMatching(const  char *input, const char *correctInput);
int32_t getTimeRemaining();

typedef struct 
{
   displayTemplate_t displayLoadTemplate;
   uint32_t displayDurationMillis;
   uint32_t displayStartMillis;
} displayQueueItem_t;

const displayQueueItem_t emptyDisplayItem = 
{
        .displayLoadTemplate = D_NONE,
        .displayDurationMillis = 0,
        .displayStartMillis = 0
};

typedef struct 
{
    uint8_t x;
    uint8_t y;
}coordinates_t;


void resetVirtualTime()
{
    virtualTime = 0;
    lastRealTime = millis();
    virtualTimeMultiplier = 1;
    
    
}

void resetGameLogic()
{
    playedAudio = 0;
    requiredActions = 0;
    resetVirtualTime();
}

/**
 * @brief Updateert de kaartcoördinaten indien ze veranderd zijn.
 *
 *
 * @param coordinates Een array van 2 uint8_t waarden: [x, y].
 *
 * @note De functie houdt een statische kopie van de laatst gebruikte coördinaten
 *       bij om onnodige updates te voorkomen.
 *
 * @note Als je INVALID_COORD gebruikt als waarde voor x of y, zal de functie de update overslaan en geen actie ondernemen. 
 */
void setMapCoordinates(const uint8_t coordinates[2])
{
    static coordinates_t lastCoordinates = {.x = INVALID_COORD, .y = INVALID_COORD};
    coordinates_t my_coordinates = {.x = coordinates[0], .y = coordinates[1]};

    if(my_coordinates.x == lastCoordinates.x && my_coordinates.y == lastCoordinates.y) return; 
    
    lastCoordinates.x = my_coordinates.x;
    lastCoordinates.y = my_coordinates.y;

    if(my_coordinates.x == INVALID_COORD || my_coordinates.y == INVALID_COORD) 
    {
        return;
    }
}

//TODO: Moet nog logia aan toe gevoegt woden
compartment_t openCompartment(const compartment_t compartment)
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

displayQueueItem_t displayQueue[2] = {emptyDisplayItem, emptyDisplayItem};
/**
 * @brief Laadt een display template.
 *
 * Roept de display driver (of printf voor debug) aan om het
 * opgegeven display template zichtbaar te maken.
 *
 * @param displayTemplate Het template dat geladen moet worden.
 *
 * @note Deze functie wordt normaal alleen intern gebruikt door
 *       de display queue mechanismen.
 */
void loadDisplayTemplate(displayTemplate_t template)
{
    //TEMP: Gebruik printf voor debug, vervang dit door echte display driver aanroepen
    if(globalSettings.censorship == NOT_CENSORED)
    {
        if(globalSettings.language == LANGUAGE_NEDERLANDS)printf(displayTemplatesNL[template]);
        else printf(displayTemplatesEn[template]);
    }
    else
    {
        if(globalSettings.language == LANGUAGE_ENGLISH)printf(displayTemplatesSafeNL[template]);
        else printf(displayTemplatesSafeEn[template]);   
    }
}
void printCustomDisplay(char *customDisplay)
{
     //TEMP: Gebruik printf voor debug, vervang dit door echte display driver aanroepen
     //TODO: Beter manier maken om customDisplays aan de que toe te voegen
    printf("Lowest becons:\n");
    printf(customDisplay);
}

/**
 * @brief Update de display queue en verwerkt de timers.
 *
 * Deze functie moet periodiek (bijvoorbeeld in de main loop) worden aangeroepen.
 * 
 * Werking:
 * - Controleert of er een actief display is.
 * - Als het huidige display net toegevoegd is (displayStartMillis == 0),
 *   wordt het geladen via loadDisplayTemplate().
 * - Als het huidige display klaar is (minimale displayduur verstreken),
 *   schuift het volgende display in de queue door naar de actieve positie.
 *
 * @note Alleen de eerste twee slots van de queue worden ondersteund.
 */
void updateDisplayQueue()
{
    
    if(displayQueue[0].displayLoadTemplate == D_NONE) return;
    
    uint32_t now = millis();
    if(displayQueue[0].displayStartMillis == 0) 
    {
        displayQueue[0].displayStartMillis = now;
        loadDisplayTemplate(displayQueue[0].displayLoadTemplate);
    }
    else if(now - displayQueue[0].displayStartMillis >= displayQueue[0].displayDurationMillis)
    {
        if(displayQueue[1].displayLoadTemplate == D_NONE)return;

        displayQueue[0] = displayQueue[1];
        displayQueue[1] = emptyDisplayItem;
    }
}
/**
 * @brief Forceert het direct tonen van een display template.
 *
 * Vervangt het huidige display in slot 0 door het opgegeven template
 * en reset de timer.
 *
 * @param displayTemplate Het template dat direct geladen moet worden.
 * @param durationMillis De minimale duur dat het template moet worden getoond.
 *
 * @note Het tweede display slot wordt geleegd bij een forceDisplay.
 * @warning Het huidige display wordt abrupt vervangen.
 */
void forceDisplayTemplate(const displayTemplate_t displayTemplate, const uint32_t durationMillis)
{
    if(displayQueue[0].displayLoadTemplate == displayTemplate) return;

    displayQueueItem_t newItem = 
    {
        .displayLoadTemplate = displayTemplate,
        .displayDurationMillis = durationMillis,
        .displayStartMillis = 0
    };
    displayQueue[0] = newItem;
    displayQueue[1] = emptyDisplayItem;
}
/**
 * @brief Voegt een display template toe aan de queue.
 *
 * Als er een leeg slot is, wordt het nieuwe template toegevoegd.
 * Controleert dat dezelfde template niet dubbel in de queue komt.
 *
 * @param displayTemplate Het template dat toegevoegd moet worden.
 * @param durationMillis De minimale duur dat het template moet worden getoond.
 *
 * @note Ondersteunt slechts twee slots in de queue.
 */
void addDisplayTemplate(const displayTemplate_t displayTemplate, const uint32_t durationMillis)
{
    if(displayQueue[0].displayLoadTemplate == displayTemplate) return; // Template is al actief, geen update nodig
    if(displayQueue[1].displayLoadTemplate == displayTemplate) return; // Template is al actief, geen update nodig

    displayQueueItem_t newItem = 
    {
        .displayLoadTemplate = displayTemplate,
        .displayDurationMillis = durationMillis,
        .displayStartMillis = 0
    };

    if(displayQueue[0].displayLoadTemplate == D_NONE)
    {
        displayQueue[0] = newItem;
    }
    else
    {
        displayQueue[1] = newItem;
    }
}
/**
 * @brief Checks if the current display template has finished playing.
 *
 * This function determines whether al display template has completed
 *
 * @return true  If the template done playing or if there is no active template. else false.
 * 
 */
bool isDisplayTemplateDonePlaying()
{
    if(displayQueue[0].displayLoadTemplate == D_NONE && displayQueue[1].displayLoadTemplate == D_NONE) return true; // Template niet in buffer
   
    uint32_t now = millis();
    if(displayQueue[0].displayLoadTemplate != D_NONE && displayQueue[0].displayStartMillis == 0) return false; // Template is net geladen, dus nog niet klaar
    if(displayQueue[1].displayLoadTemplate != D_NONE && displayQueue[1].displayStartMillis == 0) return false; // Template is net geladen, dus nog niet klaar
    if(now - displayQueue[0].displayStartMillis < displayQueue[0].displayDurationMillis) return false; // Template still dusy
    if(now - displayQueue[1].displayStartMillis < displayQueue[1].displayDurationMillis) return false; // Template still dusy
    return true; // Template is don playing
}
/**
 * @brief Stelt wlke speciale actie gedaan moet worde
 *
 *
 * @param required De speciale actie die ingesteld moet worden (uit specialActies_t).
 * @param state    de waarde die de actie moet krijgen (true = actie actief, false = actie niet actief).
 *  *
 */
void setRequiredSpecialActies(const specialActies_t required, const bool state)
{
    requiredSpecialActies = state ? required : NON_S;
    switch (required)
    {
    case FINGER_PRINT_S:
        setMustTouchSensor(state);

        break;
    case KEY_S:
        
        break;
    case SWITCH_S:

        break;
    default:
        break;
    }
}

void updateSpecialActies()
{     
    static bool lastTouchPressed = false;
    bool touchPressed = isTouchPressed();
    if(touchPressed && !lastTouchPressed) 
    {

        if(requiredSpecialActies != NON_S)
        {
            preformtSpecialAction = FINGER_PRINT_S;
            if(requiredSpecialActies != FINGER_PRINT_S)
            {
                applyWrongAnswerPenalty();
            }
        }

    }    
    lastTouchPressed = touchPressed;

    
    uint8_t lastingActions = 0;
    bool actionCorect = false;

    if(readKeySensor())       lastingActions |= (1 << KEY_S);
    if(readSwitchSensor())    lastingActions |= (1 << SWITCH_S);
    if(lastingActions == requiredActions) return;
    
    if((lastingActions & (1 << SWITCH_S)) != (requiredActions & (1 << SWITCH_S)))
    {
        if(requiredSpecialActies != NON_S)
        {
            if(requiredSpecialActies == SWITCH_S)
            {
                requiredActions |= (1 << SWITCH_S);
                actionCorect = true;
                playGlobelAudio(AUDIO_CORRECT_ANSWER);
                printf("Switch sensor activated\n"); // Debug output
            }
            preformtSpecialAction = SWITCH_S;
        }
    }
    if((lastingActions & (1 << KEY_S))!= (requiredActions & (1 << KEY_S)))
    {
        if(requiredSpecialActies != NON_S)
        {
            if(requiredSpecialActies == KEY_S)
            {
                requiredActions |= (1 << KEY_S);
                actionCorect = true;
                playGlobelAudio(AUDIO_CORRECT_ANSWER);
            }
            preformtSpecialAction = KEY_S;
        }
    }

    if(!actionCorect)
    {
       
        timeGamePanaltyMillis += MS_PER_TICK_PANALTY;
    }
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
 * 
 * @warning mogelijk lokia toe voegen voor als 2 actions tegelijktijd uitgevoert worden
 */
specialActies_t getSpecialActies()
{
    specialActies_t specialAction = preformtSpecialAction;
    preformtSpecialAction = NON_S;
    return specialAction;
}

/**
 * @brief Past de straf toe voor een fout antwoord.
 */
void applyWrongAnswerPenalty()
{
    runData.wrongAnswerCount++;
    uint32_t penalty = getWrongAnswerPenalty();
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
    uint32_t elapsedTime = (millis() - startGameMillis) + timeGamePanaltyMillis;
    return globalSettings.difficulty == WRONG_ANSWER_TIME_X2 ? getVirtualElapsedTime() : elapsedTime;
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
void updateTimeGamePenaltyMillis()
{
  
    if(timeGamePanaltyBuffer <= 0) return;

    if(timeGamePanaltyBuffer >= MS_PER_TICK_PANALTY)
    {
        timeGamePanaltyBuffer -= MS_PER_TICK_PANALTY;
        timeGamePanaltyMillis += MS_PER_TICK_PANALTY;
    }
    else
    {
        timeGamePanaltyMillis += timeGamePanaltyBuffer;
        timeGamePanaltyBuffer = 0;
    }
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
bool isInCorrectRoom(const char *beconIp)
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
    hasNewAnswer  = false;
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
    return  (elapsedTime  <= globalSettings.totalTime * 60UL * 1000UL) || (globalSettings.difficulty < 2);
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
    if(totalSec == lastSec && gameActiv) return;
    lastSec = totalSec;

    setGameTimer(lastSec);
    buzzer_play(BUZZERT_DURATION);

}

/**
 * @brief Sets the game timer display to the specified time in seconds.
 * 
 * @param sec The time to display in seconds.
 */
void setGameTimer(int32_t sec)
{
    if(sec < 0)
    {
        sec = -sec;  // maak positief voor berekening
    }

    uint16_t minutes = sec / 60;
    uint16_t seconds = sec % 60;
   printf("Time Remaining: %02d:%02d\n", minutes, seconds); // Debug output
    hexDisplay_setTime(minutes, seconds);
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
    for(uint8_t i = 0; i <MAX_ROOMS; i++) 
    {
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

        case WRONG_ANSWER_TIME_X2:
            virtualTimeMultiplier ++;
            break;

        default:
            return 0;
            break;
    }
    return timePanalty;

}

/**
 * @brief Reset de virtuele tijd naar beginwaarde.
 *
 * Zet de virtuele tijd terug naar 0 en synchroniseert deze met de huidige
 * echte tijd (`millis()`). Ook wordt de tijdsvermenigvuldiger teruggezet naar 1.
 *
 * @note Na deze functie loopt de virtuele tijd weer synchroon met de echte tijd.
 */

/**
 * @brief Update de virtuele tijd op basis van verstreken echte tijd.
 *
 * Berekent hoeveel echte tijd is verstreken sinds de laatste update en telt
 * deze op bij de virtuele tijd, vermenigvuldigd met `virtualTimeMultiplier`.
 *
 * @note Deze functie moet periodiek worden aangeroepen (bijvoorbeeld in de main loop)
 *       om de virtuele tijd correct te laten lopen.
 *
 * @warning Als deze functie niet regelmatig wordt aangeroepen, zal de virtuele tijd
 *          onnauwkeurig worden.
 */
void updateVirtualTime()
{
    uint32_t now = millis() ;
    virtualTime += (uint32_t)((now - lastRealTime) * virtualTimeMultiplier);
    lastRealTime = now;
}

/**
 * @brief Haalt de verstreken virtuele tijd op.
 *
 * @return De huidige virtuele tijd in milliseconden.
 *
 * @note De waarde is afhankelijk van hoe vaak `updateVirtualTime()` wordt aangeroepen
 *       en de ingestelde `virtualTimeMultiplier`.
 */
uint32_t getVirtualElapsedTime()
{
    return virtualTime;
}

#define TIME_DEPENDING_ADUIO_INTERVAL 5 * FROM_MIN_TO_MS
#define TIME_AUDIO_CHECK_LEN 5
#define DELAY_FROM_START_TO_FIRST_AUDIO 15 * FROM_MIN_TO_MS
typedef struct
{
    uint32_t checkTimeSec;
    audio_files_t audioToPlay;
}time_audio_check_t;

const time_audio_check_t time_audio_check[TIME_AUDIO_CHECK_LEN] = 
{
    {.checkTimeSec = 1  * FROM_MIN_TO_MS, .audioToPlay = AUDIO_1_MIN_LEFT},
    {.checkTimeSec = 5  * FROM_MIN_TO_MS, .audioToPlay = AUDIO_5_MIN_LEFT},
    {.checkTimeSec = 15 * FROM_MIN_TO_MS, .audioToPlay = AUDIO_15_MIN_LEFT},
    {.checkTimeSec = 30 * FROM_MIN_TO_MS, .audioToPlay = AUDIO_30_MIN_LEFT},
    {.checkTimeSec = 45 * FROM_MIN_TO_MS, .audioToPlay = AUDIO_45_MIN_LEFT},
};

void updateTimeDependingAudio()
{
    static uint32_t lastTimeDependingAudio = 0;
    if(globalSettings.audio == AUDIO_OFF) return;

    audio_files_t audioToPlay = AUDIO_NONE;

    uint32_t timeRemaining = getTimeRemaining();
    for(uint8_t i = 0; i < TIME_AUDIO_CHECK_LEN; i++)
    {
        if(timeRemaining <= time_audio_check[i].checkTimeSec && !(playedAudio & (1<<i)))
        {
            playedAudio |= (1 <<i);
            if(time_audio_check[i].checkTimeSec + DELAY_FROM_START_TO_FIRST_AUDIO < globalSettings.totalTime  * FROM_MIN_TO_MS) audioToPlay = time_audio_check[i].audioToPlay;
            break;
        }
    }
    
    
    if(audioToPlay == AUDIO_NONE) return;  
    uint32_t now = millis();
    if(now - lastTimeDependingAudio < TIME_DEPENDING_ADUIO_INTERVAL)  return;
    lastTimeDependingAudio = now;

    playAudio(audioToPlay);
}