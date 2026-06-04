#include "audio.h"
#include "board.h"
#include "game_data.h"
#include "time_millis.h"
#include "serial.h"
#include <stdio.h>
#include "game_logic.h"
#include "display.h"

#define BUSY_PIN 2
#define QUEUE_SIZE 20
#define SEND_DELAY 50
#define MAX_VOLUME 30
typedef struct 
{
    uint8_t  command;
    uint16_t param;
} audio_command_t;

audio_command_t audioQueue[QUEUE_SIZE] = {};
audio_command_t forceCommand = {};
uint8_t queueHead = 0;
uint8_t queueTail = 0;
uint8_t volume = 5;
uint32_t lastSendTime = 0;
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

bool isForceCommand = false;

void audio_init(void)
{

    quickPrint("audio Init");
    lupart0_init(9600);

    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);

    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);

    PORT2->PCR[BUSY_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);

   
}

/**
 * @brief Calculate checksum in cmdBuffer.
 * 
 */
void calculateChecksum(void) 
{
    uint16_t sum = 0;
    for (uint8_t i = VERSION_INFORMATION; i <= PARAM_LOW; i++) 
    {
        sum += cmdBuffer[i];
    }
    uint16_t checksum = 0 - sum;
    cmdBuffer[CHECKSUM_HIGH] = (checksum >> 8) & 0xFF;
    cmdBuffer[CHECKSUM_LOW] = checksum & 0xFF;
}

/**
 * @brief Build and send a command to the DFPlayer Mini.
 *
 * @param[in] cmdData Struct containing command and parameter data.
 */
void sendCommand(audio_command_t cmdData) 
{
    cmdBuffer[COMMAND] = cmdData.command;
    cmdBuffer[PARAM_HIGH] = (cmdData.param >> 8) & 0xFF;
    cmdBuffer[PARAM_LOW] = cmdData.param & 0xFF;
    calculateChecksum();
    
    for (uint8_t i = 0; i < CMD_LENGTH; i++) 
    {
        lupart0_putchar(cmdBuffer[i]);
    }
}

/**
 * @brief Add a new audio to the queue
 *
 * Inserts a new command with parameter into the circular queue.
 * If the queue is full, the command is silently discarded.
 *
 * @param[in] command Command byte to enqueue.
 * @param[in] param   16-bit parameter associated with the command.
 */
void enqueueCommand(uint8_t  command, uint16_t param) 
{
    uint8_t nextHead = (queueHead + 1) % QUEUE_SIZE;
    if (nextHead == queueTail) return; // Queue is vol, negeer nieuw commando

    audioQueue[queueHead].command = command;
    audioQueue[queueHead].param = param;
    queueHead = nextHead;
}

/**
 * @brief Process and send queued the audio commands.
 *
 * Sends the next command from the queue when the DFPlayer is ready.
 * Ensures a minimum delay between commands and supports a priority
 * "force command" that bypasses the queue.
 *
 * - Forced commands are sent immediately (once) when requested.
 * - Normal commands are only sent when the BUSY pin indicates ready.
 * - Commands are rate-limited using SEND_DELAY.
 * 
 * @note This function should be called regularly in the main loop
 */
void updateAudioQueue()
{
    

    if (queueHead == queueTail && !isForceCommand ) return;

    // Kleine vertraging tussen commando's
    uint32_t now = millis();
    if (now - lastSendTime < SEND_DELAY) return;
    lastSendTime = now;

    // Handle forced command (priority)
    if(isForceCommand)
    {
        isForceCommand = false;
        sendCommand(forceCommand);
        return;
    }

    // Wait until DFPlayer is ready (BUSY pin inactive)
    if (!getPinState(GPIO2, BUSY_PIN)) return;
    

    sendCommand(audioQueue[queueTail]);
    
    queueTail = (queueTail + 1) % QUEUE_SIZE; 
}

/**
 * @brief Queue command to play a specific audio file in a folder.
 * *
 * @param[in] file  Folder number.
 * @param[in] audio File number within the folder.
 */
void audioPlayInFile(uint8_t file, uint8_t audio)
{
    enqueueCommand(CMD_SPECIFIC_FOLDER, ((uint16_t)file << 8) | audio);
}


/**
 * @brief Set audio volume.
 *
 * Clamps volume to MAX_VOLUME, updates global volume,
 * and enqueues the command.
 *
 * @param[in] vol Desired volume level.
 */
void audioSetVolume(uint8_t vol)
{
    if (vol > MAX_VOLUME) vol = MAX_VOLUME;
    enqueueCommand(CMD_SET_VOLUME, vol);
    volume = vol;
}


/**
 * @brief Play a global audio file.
 *
 * Plays audio from the global folder.
 * then queues the audio file 
 *
 * @param[in] audioFile Global audio file identifier.
 */
void playGlobelAudio(globel_audio_files_t audioFile)
{
    if (globalSettings.audio == AUDIO_OFF || volume == 0) return;
    audioPlayInFile(GLOBEL_FOLDER, audioFile);
}


/**
 * @brief Play a language-dependent audio file.
 *
 * Selects folder based on language and censorship settings,
 * then queues the audio file 
 *
 * @param[in] audioFile Audio file identifier.
 */
void playAudio(audio_files_t audioFile)
{
    if (globalSettings.audio == AUDIO_OFF || volume == 0 || audioFile == AUDIO_NONE) return;
    
    uint8_t file = (globalSettings.language == LANGUAGE_ENGLISH) ? ENGLISH_FOLDER : NEDERLANDS_FOLDER;
    audioPlayInFile(file + globalSettings.censorship, audioFile);
}

/**
 * @brief Force stop current audio playback.
 *
 * Sets a high-priority stop command that bypasses the queue.
 */
void stopAudio(void)
{
    if (globalSettings.audio == AUDIO_OFF || volume == 0) return;

    isForceCommand = true;
    forceCommand.command = CMD_STOP;
    forceCommand.param   = 0;
}


/**
 * @brief Force play a global audio file.
 *
 * Immediately schedules a global audio command with priority,
 * bypassing the queue.
 *
 * @param[in] audioFile Global audio file identifier.
 */
void forceGlobelAudio(globel_audio_files_t audioFile)
{
    if (globalSettings.audio == AUDIO_OFF || volume == 0 || audioFile == AUDIO_NONE_G) return;

    isForceCommand = true;
    forceCommand.command = CMD_SPECIFIC_FOLDER;
    forceCommand.param   = ((uint16_t)GLOBEL_FOLDER << 8) | audioFile;
}


/**
 * @brief Force play a language-dependent audio file.
 *
 * Selects folder based on language settings and schedules
 * the command with priority, bypassing the queue.
 *
 * @param[in] audioFile Audio file identifier.
 */
void forceplayAudio(audio_files_t audioFile)
{
    if (globalSettings.audio == AUDIO_OFF || volume == 0 || audioFile == AUDIO_NONE) return;

    isForceCommand = true;

    uint8_t file = (globalSettings.language == LANGUAGE_ENGLISH) ? ENGLISH_FOLDER : NEDERLANDS_FOLDER;

    forceCommand.command = CMD_SPECIFIC_FOLDER;
    forceCommand.param   = ((uint16_t)file << 8) | audioFile;
}