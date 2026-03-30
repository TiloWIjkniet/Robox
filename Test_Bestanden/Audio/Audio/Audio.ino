// ===============================
// DFPlayer Mini - Queue Controller
// ===============================

// DFPlayer Mini BUSY pin
#define BUSY_PIN 13

// Command array lengte en queue grootte
#define CMD_LENGTH 10
#define QUEUE_SIZE 20

// DFPlayer command array indexes
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

// Struct voor één audio commando
typedef struct 
{
    uint8_t  command;
    uint16_t param;
} AudioCommand;

AudioCommand audioQueue[QUEUE_SIZE] = {};
uint8_t queueHead = 0;
uint8_t queueTail = 0;

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
        Serial.write(cmdBuffer[i]);
    }
}



// Voeg commando toe aan de queue
void enqueueCommand(uint8_t  command, uint16_t param) 
{
    audioQueue[queueHead] = { command, param };
    queueHead++;
    queueHead = queueHead % QUEUE_SIZE; // cirkelende queue
}

// Verwerk queue: stuur volgend commando als DFPlayer klaar is
void processQueue() 
{
    static uint32_t lastSendTime = 0;

    // Niets te doen als queue leeg is
    if (queueHead == queueTail) return;

    // Wacht tot DFPlayer klaar is (BUSY pin HIGH = geen audio)
    if (!digitalRead(BUSY_PIN)) return;

    // Kleine vertraging tussen commando's
    uint32_t now = millis();
    if (now - lastSendTime < 1000) return;
    lastSendTime = now;

    // Stuur volgende commando in queue
    sendCommand(audioQueue[queueTail]);
    queueTail++;
    queueTail = queueTail % QUEUE_SIZE; // cirkelende queue
}

void audioPlay(uint8_t audio)
{
  enqueueCommand(CMD_SPECIFIC_TRACK, audio);
}

void audioPlayInFile(uint8_t file, uint8_t audio)
{
  enqueueCommand(CMD_SPECIFIC_FOLDER, ((uint16_t)file << 8) | audio);
}
#define GLOBEL_FOLDER               1
#define ENGLISH_FOLDER              2
#define ENGLISH_CENSORED_FOLDER     3
#define NEDERLANDS_FOLDER           4
#define NEDERLANDS_CENSORED_FOLDER  5

typedef enum
{
  WRONG_ANSWER = 1,   
  CORRECT_ANSWER, 
} globel_audio_files_t;

typedef enum
{
  BOM_HAS_BEEN_PLANTED = 1, 
  BOM_HAS_BEEN_DEFUSED, 
  TIME_IS_UP
} audio_files_t;

void setup() 
{
    Serial.begin(9600);
    pinMode(BUSY_PIN, INPUT_PULLUP);

    cmdBuffer[FEEDBACK] = 1; 
    enqueueCommand(CMD_QUERY_TOTAL_TRACKS, 0);
    
    // Voeg een paar testcommando's toe
    enqueueCommand(CMD_SET_VOLUME, 10);
    audioPlayInFile(GLOBEL_FOLDER,               WRONG_ANSWER);
    audioPlayInFile(GLOBEL_FOLDER,               CORRECT_ANSWER);

    audioPlayInFile(ENGLISH_FOLDER,              BOM_HAS_BEEN_PLANTED);
    audioPlayInFile(ENGLISH_FOLDER,              BOM_HAS_BEEN_DEFUSED);
    audioPlayInFile(ENGLISH_FOLDER,              TIME_IS_UP);

    audioPlayInFile(ENGLISH_CENSORED_FOLDER,     BOM_HAS_BEEN_PLANTED);
    audioPlayInFile(ENGLISH_CENSORED_FOLDER,     BOM_HAS_BEEN_DEFUSED);
    audioPlayInFile(ENGLISH_CENSORED_FOLDER,     TIME_IS_UP);

    audioPlayInFile(NEDERLANDS_FOLDER,           BOM_HAS_BEEN_PLANTED);
    audioPlayInFile(NEDERLANDS_FOLDER,           BOM_HAS_BEEN_DEFUSED);
    audioPlayInFile(NEDERLANDS_FOLDER,           TIME_IS_UP);

    audioPlayInFile(NEDERLANDS_CENSORED_FOLDER,  BOM_HAS_BEEN_PLANTED);
    audioPlayInFile(NEDERLANDS_CENSORED_FOLDER,  BOM_HAS_BEEN_DEFUSED);
    audioPlayInFile(NEDERLANDS_CENSORED_FOLDER,  TIME_IS_UP);

}



void loop() 
{
    processQueue(); // verwerk queue telkens als DFPlayer klaar is
}