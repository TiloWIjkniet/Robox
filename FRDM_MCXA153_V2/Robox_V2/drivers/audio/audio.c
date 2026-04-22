#include "audio.h"
#include " gpio.h"
#include "lupart0.h"
#include "time.h"

#define BUSY_PIN 2

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

#define AUDIO_PLAYING 0
#define AUDIO_IN_QUEUE 1
#define AUDIO_QUEUE_LEN 2

#define AUDIO_SEND_DELAY 50

#define UN_SKIPPABLE 0x001
#define FORCE 0x010
#define PLAY_ONLY_RIGHT_AWAY 0x100



 typedef struct
 {
    uint8_t cmd,
    uint8_t parm1,
    uint8_t parm2,
 } audioCommand_t;

 audioCommand_t audioQueue[AUDIO_QUEUE_LEN] = {};

void audio_init(void)
{
    pin_init(GPIO2, BUSY_PIN, INPUT, 0);
}

static inline bool audio_isPlaying(void)
{
    return getPinState(GPIO2, BUSY_PIN);
}

 void audio_sendCommand(audioCommand_t audioCommand)
 {

 }
void audio_play()
{

}

void audio_update(void)
{

}

