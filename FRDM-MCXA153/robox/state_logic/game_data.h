#ifndef GAME_DATA_H
#define GAME_DATA_H

#define MAX_RUNS 20
#define MAX_ROOMS 20
#define MAX_CHAR_IN_STRING 50
#define MAX_ANSWERS 5
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef struct 
{
    uint16_t roomTimes[MAX_ROOMS];
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
uint32_t timeGamePenaltyMillis;

uint32_t startGameMillis;

char answerBuffer[MAX_CHAR_IN_STRING];

bool hasNewAnswer = false;
#endif