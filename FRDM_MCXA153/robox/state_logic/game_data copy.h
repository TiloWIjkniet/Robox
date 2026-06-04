#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <stdbool.h>
#include <stdint.h>

#define DEBUG_ON_PC 0
#if DEBUG_ON_PC
#include <stdio.h>
#endif

#define INVALID_COORD 255

#define MAX_RUNS 20
#define MAX_ROOMS 20

#define MAX_CHAR_IN_STRING 20
#define MAX_CHAR_IN_STRING_LONG 50

#define MAX_ANSWERS 5
#define MAX_DIFFICULTY 5

#define FROM_MIN_TO_MS 60 *1000


typedef struct 
{
    float roomTimes[MAX_ROOMS];
    uint8_t wrongAnswerCnt;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;

}runData_t;

typedef enum // let op ook op esp aanpassen
{
    NON_C,
    ALL_C
}compartment_t;
typedef enum  // let op ook op esp aanpassen
{
    NON_S,
    SWITCH_S,
    KEY_S,
    FINGER_PRINT_S
}specialActies_t;

typedef struct 
{
    uint8_t coordinates[2];
    char beconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING]; // Kan nog verklijt worden MAX_ANSWERSMAX_ANSWERS * antowwrd = 20 dus kan een stuk minder data in answers
    compartment_t openCompartment;
    specialActies_t specialActies;
    char roomNaam[MAX_CHAR_IN_STRING];
    
}roomSettings_t;

typedef enum
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,     // 1 minuut aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,     // 5 minuten aftrek, spel gaat door
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,   // Tijd aftrekken (bijv. 5 min), stop als tijd 0
    WRONG_ANSWER_TIME_X2 
}wrongAnswerPenalty_t;

typedef enum
{      
    AUDIO_ON,
    AUDIO_OFF
}audio_t;

typedef enum
{
    NOT_CENSORED,    // nog niet gecensureerd
    CENSORED         // al gecensureerd
} censorship_status_t;

typedef enum
{
    LANGUAGE_ENGLISH,
    LANGUAGE_NEDERLANDS
} language_t;

typedef struct 
{
    wrongAnswerPenalty_t difficulty;
    uint16_t totalTime;
    audio_t audio;
    language_t language;
    censorship_status_t censorship;

}globalSettings_t;

//Om geen cordinaten te laten zien
extern const uint8_t NO_COORDINATES[2];

extern  globalSettings_t globalSettings;
extern  runData_t runData;
extern  roomSettings_t roomsSettings[MAX_ROOMS];
extern  runData_t runDatas[MAX_RUNS];

extern  uint32_t timeGamePanaltyBuffer;
extern  uint32_t timeGamePanaltyMillis;
extern  uint32_t startGameMillis;
extern  uint8_t roomIndex;

extern  char answerBuffer[MAX_CHAR_IN_STRING];
extern  char inputBuffer[MAX_CHAR_IN_STRING];
extern  bool hasNewAnswer;
extern bool isGameActiv;

#endif