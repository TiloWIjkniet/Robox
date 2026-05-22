#ifndef DATA_H
#define DATA_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_ROOMS 20
#define MAX_DIFFICULTY 5

#define MAX_ANSWERS 5
#define MAX_CHAR_IN_STRING 16

typedef enum 
{
    NON_C,
    ALL_C
}compartment_t;
typedef enum
{
    NON_S,
    SWITCH_S,
    KEY_S,
    FINGER_PRINT_S
}specialActions_t;

typedef enum
{      
    AUDIO_ON,
    AUDIO_OFF
}audio_t;

typedef enum
{
    LANGUAGE_ENGLISH,
    LANGUAGE_NEDERLANDS
} language_t;

typedef enum
{
    WRONG_ANSWER_MINUS_1MIN_CONTINUE,    
    WRONG_ANSWER_MINUS_5MIN_CONTINUE,     
    WRONG_ANSWER_MINUS_5MIN_STOP,
    WRONG_ANSWER_MINUS_15MIN_STOP,  
    WRONG_ANSWER_TIME_X2 
}wrongAnswerPenalty_t;

typedef struct 
{
    float roomTimes[MAX_ROOMS];
    uint8_t wrongAnswerCnt;
    uint8_t totalTime;
    uint8_t difficulty;
    uint8_t maxRooms;

}runData_t;

typedef struct 
{
    uint8_t coordinates[2];
    char beaconIp[MAX_CHAR_IN_STRING];
    char answers[MAX_ANSWERS][MAX_CHAR_IN_STRING]; 
    compartment_t openCompartment;
    specialActions_t specialActions;
    char roomName[MAX_CHAR_IN_STRING];
    
}roomSettings_t;

typedef struct 
{
    wrongAnswerPenalty_t difficulty;
    uint16_t totalTime;
    audio_t audio;
    language_t language;
    censorship_status_t censorship;

}globalSettings_t;

extern globalSettings_t globalSettings;
extern roomSettings_t roomsSettings[MAX_ROOMS];
extern runData_t runData;



#endif