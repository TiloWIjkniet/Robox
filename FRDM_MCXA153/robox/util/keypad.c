#include "keypad.h"
#include "board.h"
#include <stdio.h>
#include "time_millis.h"
#include "buzzer.h"
#define ROWS 4
#define COLS 3

#define DEBOUNCE_TIME 50


#define PIN_COLOM_0 10
#define PIN_COLOM_1 6
#define PIN_COLOM_2 8
s
#define PIN_ROW_1 31
#define PIN_ROW_2 11
#define PIN_ROW_3 9

const uint8_t keymap[ROWS][COLS] =
{
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'},
  {'#','0','*'}
};

typedef struct 
{
    bool pressed;
    bool lastRawState;
    uint32_t lastChange;
} keyState_t;

keyState_t keys[ROWS][COLS];

char inputBuffer[CHAR_IN_STRING] ="";

char answerBuffer[CHAR_IN_STRING];
bool hasNewAnswer;
uint8_t inputBufferIndex = 0;

char getKey();

void keyPad_init()
{


    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1); 

    PORT3->PCR[PIN_COLOM_0] = PORT_PCR_LK(1);
    PORT3->PCR[PIN_COLOM_1] = PORT_PCR_LK(1);
    PORT3->PCR[PIN_COLOM_2] = PORT_PCR_LK(1);

    GPIO3->PDOR |= (1<<PIN_COLOM_0) | (1<<PIN_COLOM_1) | (1<<PIN_COLOM_2);
    GPIO3->PDDR |= (1<<PIN_COLOM_0) | (1<<PIN_COLOM_1) | (1<<PIN_COLOM_2);

    PORT3->PCR[PIN_ROW_0] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORT3->PCR[PIN_ROW_1] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORT3->PCR[PIN_ROW_2] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORT3->PCR[PIN_ROW_3] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(0) | PORT_PCR_PE(1) | PORT_PCR_PS(1);


}

bool getPinState(uint8_t pin)
{
  return ((GPIO3->PDIR & (1<<pin)) == 0);
}

void setPintate(uint8_t pin, bool value)
{
  if(!value)GPIO3->PCOR = (1<<pin);
  else GPIO3->PSOR = (1<<pin);
}

/**
 * @brief Leegt de volledige inputbuffer en reset de index.
 *
 * Deze functie wist alle inhoud van inputBuffer door het geheugen
 * op nul te zetten met memset(). Daarna wordt inputBufferIndex
 * teruggezet naar 0 en wordt de buffer expliciet afgesloten met
 * een null-terminator ('\0').
 *
 * Na aanroep is inputBuffer een lege, geldige C-string.
 *
 */
void emptyInputBuffer()
{
  memset(&inputBuffer, 0, sizeof(inputBuffer));
  inputBufferIndex = 0;
  hasNewAnswer = false;
  inputBuffer[inputBufferIndex] = '\0';
}

/**
 * @brief Verwerkt een ingedrukte toets en past de inputbuffer aan.
 *
 * Deze functie leest een toets via getKey() en verwerkt deze volgens
 * de volgende regels:
 *
 * - ' '  : Geen toets ingedrukt → geen actie.
 * - '#'  : Leegt de volledige inputbuffer.
 * - '*'  : Verwijdert het laatste karakter uit de inputbuffer (backspace).
 * - default : Voegt het ontvangen karakter toe aan de inputbuffer
 *             mits er nog ruimte beschikbaar is.
 *
 * De functie zorgt ervoor dat inputBuffer altijd correct wordt
 * afgesloten met een null-terminator ('\0'), zodat deze veilig
 * als C-string gebruikt kan worden.
 *
 */
void updateInputBuffer()
{
  char key = getKey();
  switch (key) 
  {
    case ' ':
      return;
    break;
    case '#':
        strncpy(answerBuffer, inputBuffer, sizeof(answerBuffer) - 1);
        answerBuffer[sizeof(answerBuffer) - 1] = '\0';
        if(inputBufferIndex <= 0) return;
        hasNewAnswer = true;

        inputBufferIndex = 0;
        inputBuffer[inputBufferIndex] = '\0';
    break;
    case '*':
      if(inputBufferIndex > 0 )inputBufferIndex --;
      inputBuffer[inputBufferIndex] = '\0';
    break;
    default:
      inputBuffer[inputBufferIndex] = key;
      if(inputBufferIndex < CHAR_IN_STRING )inputBufferIndex ++;
      inputBuffer[inputBufferIndex] = '\0';
    break;
  }
  buzzer_play(BUZZERT_DURATION);
  printf("Input buffer: %s\n", inputBuffer);
}

/**
 * @brief Scant het keypad en geeft de toets terug die stabiel ingedrukt is.
 * 
 * Deze functie doorloopt alle kolommen en rijen van de keypad. 
 * Voor elke toets wordt gecontroleerd of de huidige pinstatus verschilt van de laatst bekende 
 * stabiele status. Als er een verandering is en deze langer dan DEBOUNCE_TIME ms stabiel blijft,
 * wordt deze geregistreerd als nieuwe toestand. Bij een stabiele druk van een toets
 * wordt het corresponderende karakter uit de keymap teruggegeven.
 * 
 * @return char Het karakter van de ingedrukte toets bij een stabiele druk. 
 *              Als er geen toets is ingedrukt, wordt een spatie ' ' teruggegeven.
 * 
 * @note 
 * - Debouncing gebeurt door het controleren van de tijd sinds de laatste verandering
 *   van elke toets (keys[row][col].lastChange).
 * - De functie werkt niet met meerdere gelijktijdig ingedrukte toetsen; de eerste stabiele toets
 *   wordt teruggegeven.
 * - De keymap wordt gebruikt om van (row, col) naar het juiste karakter te vertalen.
 * - `setPintate(pin, value)` en `getPinState(pin)` moeten beschikbaar zijn voor het aansturen en lezen
 *   van de keypad-pinnen.
 * - Deze functie moet regelmatig aangeroepen worden in de loop() voor correcte detectie.
 */
char getKey()
{ 
  const uint8_t pin_coloms[COLS]  = {PIN_COLOM_0, PIN_COLOM_1, PIN_COLOM_2};
  const uint8_t pin_rows[ROWS]    = {PIN_ROW_0, PIN_ROW_1, PIN_ROW_2, PIN_ROW_3};

  for(uint8_t col = 0; col < COLS; col++)
  {

    for(int c = 0; c < COLS; c++) setPintate(pin_coloms[c], true);
    setPintate(pin_coloms[col], false);
    for (uint8_t row =0; row < ROWS; row++)
    {
      bool pinState = getPinState(pin_rows[row]);
       uint32_t now  = millis();
      if(pinState != keys[row][col].lastRawState) 
      {
         keys[row][col].lastRawState = pinState;
         keys[row][col].lastChange = now;
      }

      if(now - keys[row][col].lastChange >= DEBOUNCE_TIME)
      {
        
        if(keys[row][col].pressed != pinState)
        {
            keys[row][col].pressed = pinState;

            if(pinState)
            {
                return keymap[row][col];
            }
        }
      }
      
    }
  }
  return ' ';
}


