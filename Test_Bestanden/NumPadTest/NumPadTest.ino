#include <string.h>

//midelste,	boven,	rechts,	onder,	links,	mide onder, 	midde bovem
#define ROWS 4
#define COLS 3

#define DEBOUNCE_TIME 40
#define CHAR_IN_STRING 50

#define PIN_COLOM_0 6
#define PIN_COLOM_1 2
#define PIN_COLOM_2 4

#define PIN_ROW_0 3
#define PIN_ROW_1 8
#define PIN_ROW_2 7
#define PIN_ROW_3 5

char keymap[ROWS][COLS] =
{
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'},
  {'#','0','*'}
};

typedef struct 
{
    bool pressed;
    uint32_t lastChange;
} keyState_t;

keyState_t keys[ROWS][COLS];

char inputBuffer[CHAR_IN_STRING] ="";
uint8_t inputBufferIndex = 0;

void setup() 
{
 // Kolom pinnen (bijvoorbeeld uitgaande kolommen)
    pinMode(PIN_COLOM_0, OUTPUT);
    pinMode(PIN_COLOM_1, OUTPUT);
    pinMode(PIN_COLOM_2, OUTPUT);

    // Rij pinnen (bijvoorbeeld ingangen van de matrix)
    pinMode(PIN_ROW_0, INPUT_PULLUP);
    pinMode(PIN_ROW_1, INPUT_PULLUP);
    pinMode(PIN_ROW_2, INPUT_PULLUP);
    pinMode(PIN_ROW_3, INPUT_PULLUP);

  Serial.begin(9600);
}
void loop()
{
  char key = getKey();
  switch (key) 
  {
    case ' ':
      return;
    break;
    case '#':
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
  Serial.println(inputBuffer);
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
      if(pinState != keys[row][col].pressed) 
      {
        uint32_t now  = millis();
        if(now - keys[row][col].lastChange >= DEBOUNCE_TIME)
        {
          keys[row][col].lastChange = now;
          keys[row][col].pressed = pinState;
          if(pinState == true) 
          {
            return keymap[row][col];
          }
        }
      }
    }
  }
  return ' ';
}


bool getPinState(uint8_t pin)
{
  return digitalRead(pin) == LOW;
}

void setPintate(uint8_t pin, bool value)
{
  digitalWrite(pin,value);
}