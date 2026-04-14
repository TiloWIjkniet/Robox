#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#define CMD_LENGTH 10


#define START_BYTE          0
#define VERSION_INFORMATION 1
#define LEN                 2
#define COMMAND             3
#define FEEDBACK            4
#define PARAM_HIGH          5
#define PARAM_LOW           6
#define CHECKSUM_HIGH       7
#define CHECKSUM_LOW        8
#define END_BYTE            9

// DFPlayer Mini command codes
#define CMD_SPECIFIC_TRACK        0x03
#define CMD_NEXT_TRACK            0x01
#define CMD_PREVIOUS_TRACK        0x02
#define CMD_SET_VOLUME            0x06
#define CMD_INCREASE_VOLUME       0x04
#define CMD_DECREASE_VOLUME       0x05
#define CMD_SPECIFIC_FOLDER       0x0F
#define CMD_STOP                  0x16
#define CMD_PAUSE                 0x0E
#define CMD_RESUME                0x0D
#define CMD_REPEAT_TRACK          0x11
#define CMD_REPEAT_ALL            0x12
#define CMD_RANDOM                0x18
#define CMD_RESET_MODULE          0x0C
#define CMD_QUERY_STATUS          0x42
#define CMD_QUERY_VOLUME          0x43
#define CMD_QUERY_TOTAL_TRACKS    0x48


#define GLOBEL_FOLDER               1
#define ENGLISH_FOLDER              2
#define ENGLISH_CENSORED_FOLDER     3
#define NEDERLANDS_FOLDER           4
#define NEDERLANDS_CENSORED_FOLDER  5

typedef enum
{
  AUDIO_NONE_G,
  AUDIO_WRONG_ANSWER = 1,   
  AUDIO_CORRECT_ANSWER, 
  AUDIO_AUDIO_CHECK,
  AUDIO_NUMPAD_INPUT,
  AUDIO_ERROR,                
  AUDIO_TOUCH_SENSOR_RUNNING, 
  AUDIO_TOUCH_SENSOR_CORRECT, 
  AUDIO_TOUCH_SENSOR_WRONG,
  AUDIO_OPEN_COMPARTMENT,    //TODO: Audio hier voor maken 
  AUDIO_CORRECT_ROOM         //TODO: Audio hier voor maken 
} globel_audio_files_t;

typedef enum
{
  AUDIO_NONE,
  AUDIO_BOM_HAS_BEEN_PLANTED = 1, 
  AUDIO_BOM_HAS_BEEN_DEFUSED, 
  AUDIO_TIME_IS_UP,
  AUDIO_HAFE_WAY,     //TODO: Audio hier voor maken 
  AUDIO_1_MIN_LEFT,   //TODO: Audio hier voor maken 
  AUDIO_5_MIN_LEFT,   //TODO: Audio hier voor maken 
  AUDIO_15_MIN_LEFT,  //TODO: Audio hier voor maken 
  AUDIO_30_MIN_LEFT,  //TODO: Audio hier voor maken 
  AUDIO_45_MIN_LEFT   //TODO: Audio hier voor maken 
} audio_files_t;


void updateAudioQueue(void);
void audio_init(void);

void audioSetVolume(uint8_t vol);
void playGlobelAudio(globel_audio_files_t audioFile);
void playAudio(audio_files_t audioFile);
void forceGlobelAudio(globel_audio_files_t audioFile);
void stopAudio(void);

extern uint8_t volume;
#endif

