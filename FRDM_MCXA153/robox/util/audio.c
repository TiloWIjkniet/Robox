#include "audio.h"
#include "board.h"
#include "game_data.h"
#include "time_millis.h"
#include "serial.h"
#include <stdio.h>

#define BUSY_PIN 2
#define QUEUE_SIZE 20

typedef struct 
{
    uint8_t  command;
    uint16_t param;
} AudioCommand;

AudioCommand audioQueue[QUEUE_SIZE] = {};
uint8_t queueHead = 0;
uint8_t queueTail = 0;
uint8_t volume = 20;

uint8_t cmdBuffer[CMD_LENGTH] = 
{
    0x7E,  // Start byte
    0xFF,  // Version
    0x06,  // Length (bytes after LEN)
    0x03,  // Command placeholder (wordt later gezet)
    0x00,  // Feedback (0 = geen, 1 = wel)
    0x00,  // Parameter high byte
    0x01,  // Parameter low byte
    0x00,  // Checksum high
    0x00,  // Checksum low
    0xEF   // End byte
};

void audio_init()
{

    serial_init(9600);
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);

    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);

    PORT2->PCR[BUSY_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);

    audioSetVolume(volume);
}

void calculateChecksum() 
{
    uint16_t sum = 0;
    for (uint8_t i = VERSION_INFORMATION; i <= PARAM_LOW; i++) {
        sum += cmdBuffer[i];
    }
    uint16_t checksum = 0 - sum;
    cmdBuffer[CHECKSUM_HIGH] = (checksum >> 8) & 0xFF;
    cmdBuffer[CHECKSUM_LOW] = checksum & 0xFF;
}

void sendCommand(AudioCommand cmdData) 
{
    cmdBuffer[COMMAND] = cmdData.command;
    cmdBuffer[PARAM_HIGH] = (cmdData.param >> 8) & 0xFF;
    cmdBuffer[PARAM_LOW] = cmdData.param & 0xFF;
    calculateChecksum();
    
    for (uint8_t i = 0; i < CMD_LENGTH; i++) 
    {
        serial_putchar(cmdBuffer[i]);
    }
}

// Voeg commando toe aan de queue
void enqueueCommand(uint8_t  command, uint16_t param) 
{
    uint8_t nextHead = (queueHead + 1) % QUEUE_SIZE;
    if (nextHead == queueTail) return; // Queue is vol, negeer nieuw commando

    audioQueue[queueHead].command = command;
    audioQueue[queueHead].param = param;
    queueHead = nextHead;
}

bool getPinStateAudio()
{
    return !((GPIO2->PDIR & (1<<BUSY_PIN)) == 0);
}
// Verwerk queue: stuur volgend commando als DFPlayer klaar is
void updateAudioQueue()
{
    static uint32_t lastSendTime = 0;

    if (queueHead == queueTail) return;

    // Kleine vertraging tussen commando's
    uint32_t now = millis();
    if (now - lastSendTime < 100) return;
    lastSendTime = now;

    if (!getPinStateAudio()) return;
    
    // Stuur volgende commando in queue
    // printf("head %d, tail %d\n", queueHead, queueTail);
    sendCommand(audioQueue[queueTail]);
    
    queueTail = (queueTail + 1) % QUEUE_SIZE; // cirkelende queue
}

void audioPlayInFile(uint8_t file, uint8_t audio)
{
  enqueueCommand(CMD_SPECIFIC_FOLDER, ((uint16_t)file << 8) | audio);
}



void audioSetVolume(uint8_t vol)
{
    if(vol > 30) vol = 30;
    enqueueCommand(CMD_SET_VOLUME, vol);
    volume = vol;
}

void playGlobelAudio(globel_audio_files_t audioFile)
{
    if(globalSettings.audio == AUDIO_OFF ||volume == 0 ) return;
    audioPlayInFile(GLOBEL_FOLDER, audioFile);
}

void playAudio(audio_files_t audioFile)
{

    if(globalSettings.audio == AUDIO_OFF || volume == 0) return;
    
    uint8_t file = (globalSettings.language == LANGUAGE_ENGLISH) ? ENGLISH_FOLDER : NEDERLANDS_FOLDER;
    audioPlayInFile(file + globalSettings.censorship, audioFile);
}