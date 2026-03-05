#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <stdbool.h>
#include <stdint.h>


#define DEBUG_ON_PC 1
#if DEBUG_ON_PC
#include <stdio.h>
#endif

#define INVALID_COORD 255
#define MAX_RUNS 20
#define MAX_ROOMS 20
#define MAX_CHAR_IN_STRING 50
#define MAX_ANSWERS 5
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


typedef struct 
{
    float roomTimes[MAX_ROOMS];
    uint8_t wrongAnswerCount;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;
}runData_t;

typedef enum
{
    NON_C,
    ONE_C,
    TWO_C
}compartment_t;
typedef enum
{
    NON_S,
    TOUCH_SENSOR,
    TWO_S
}specialActies_t;

typedef struct 
{
    uint8_t coordinates[2];
    char beconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING];
    compartment_t openCompartment;
    specialActies_t specialActies;
    
}roomSettings_t;

typedef enum
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,     // 1 minuut aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,     // 5 minuten aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,   // Tijd aftrekken (bijv. 5 min), stop als tijd 0
    WRONG_ANSWER_HALF_REMAINING_STOP 
}wrongAnswerPenalty_t;

typedef enum
{
    AUDIO_ON,    
    AUDIO_CENSORED,    
    AUDIO_OFF
}audio_t;

typedef struct 
{
    wrongAnswerPenalty_t difficulty;
    float totalTime;
    audio_t audio;

}globalSettings_t;


extern  globalSettings_t globalSettings;
extern  runData_t runData;
extern  roomSettings_t roomsSettings[MAX_ROOMS];

extern  uint32_t timeGamePanaltyBuffer;
extern  uint32_t timeGamePenaltyMillis;
extern  uint32_t timeRoomPanaltyMillis;
extern  uint32_t startGameMillis;
extern  uint8_t roomIndex;

extern  char answerBuffer[MAX_CHAR_IN_STRING];
extern  bool hasNewAnswer;
extern bool gameActiv;

#endif