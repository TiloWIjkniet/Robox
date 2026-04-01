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
  WRONG_ANSWER = 1,   
  CORRECT_ANSWER, 
  AUDIO_CHECK,
  NUMPAD_INPUT
} globel_audio_files_t;

typedef enum
{
  BOM_HAS_BEEN_PLANTED = 1, 
  BOM_HAS_BEEN_DEFUSED, 
  TIME_IS_UP
} audio_files_t;


void updateAudioQueue();
void audio_init();

void audioSetVolume(uint8_t vol);
void playGlobelAudio(globel_audio_files_t audioFile);
void playAudio(audio_files_t audioFile);

extern uint8_t volume;
#endif

