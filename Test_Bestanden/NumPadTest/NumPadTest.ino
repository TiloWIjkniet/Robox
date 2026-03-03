#include <string.h>

//midelste,	boven,	rechts,	onder,	links,	mide onder, 	midde bovem

#define COLOM_0 0 //links 4 
#define COLOM_1 1 //middle 0 
#define COLOM_2 2 //recht 2

#define ROW_0 3 //boven 1
#define ROW_1 4 // M_boven 6
#define ROW_2 5 // M_onder 5
#define ROW_3 6 //onder 3

#define PIN_COLOM_0 2
#define PIN_COLOM_1 3
#define PIN_COLOM_2 4

#define PIN_ROW_0 5
#define PIN_ROW_1 6
#define PIN_ROW_2 7
#define PIN_ROW_3 8


#define DEBOUNCE_TIME 20   // 20 ms
#define CHAR_IN_STRING 50
char inputBuffer[CHAR_IN_STRING];
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

  inputBuffer[DEBOUNCE_TIME] = '\0';
  //Serial.printf("%s\n", inputBuffer);

  uint8_t inputs = 0;



  const uint8_t pin_coloms[3] = {PIN_COLOM_0, PIN_COLOM_1, PIN_COLOM_2};
  const uint8_t coloms[3] =     {COLOM_0, COLOM_1, COLOM_2};
  for(int i = 0; i < 3; i++)
  {
    digitalWrite(pin_coloms[0], LOW);
    digitalWrite(pin_coloms[1], LOW);
    digitalWrite(pin_coloms[2], LOW);
    digitalWrite(pin_coloms[i], LOW);
    
    if(getPinState(PIN_ROW_0)) inputs |= (1 << ROW_0);// | (1 << coloms[i]);
    else if(getPinState(PIN_ROW_1)) inputs |= (1 << ROW_1);// | (1 << coloms[i]);
    else if(getPinState(PIN_ROW_2)) inputs |= (1 << ROW_2);// | (1 << coloms[i]);
    else if(getPinState(PIN_ROW_3)) inputs |= (1 << ROW_3);// | (1 << coloms[i]);

  }
  Serial.println(inputs, BIN);

  uint16_t buttonsPrest = 0;
  if(inputs & (1 << ROW_0))
  {
    if(inputs & (1 << COLOM_0) ) buttonsPrest |= (1 << 1);
    if(inputs & (1 << COLOM_1) ) buttonsPrest |= (1 << 2);
    if(inputs & (1 << COLOM_2) ) buttonsPrest |= (1 << 3);
  }

  if(inputs & (1 << ROW_1))
  {
    if(inputs & (1 << COLOM_0) ) buttonsPrest |= (1 << 4);
    if(inputs & (1 << COLOM_1) ) buttonsPrest |= (1 << 5);
    if(inputs & (1 << COLOM_2) ) buttonsPrest |= (1 << 6);
  }

  if(inputs & (1 << ROW_2))
  {
    if(inputs & (1 << COLOM_0) ) buttonsPrest |= (1 << 7);
    if(inputs & (1 << COLOM_1) ) buttonsPrest |= (1 << 8);
    if(inputs & (1 << COLOM_2) ) buttonsPrest |= (1 << 9);
  }

  if(inputs & (1 << ROW_3))
  {
    if(inputs & (1 << COLOM_0) ) buttonsPrest |= (1 << 10);
    if(inputs & (1 << COLOM_1) ) buttonsPrest |= (1 << 0);
    if(inputs & (1 << COLOM_2) ) buttonsPrest |= (1 << 11);
  }

  
  
  debounceButton(buttonsPrest, 1,  '1');
  debounceButton(buttonsPrest, 2,  '2');
  debounceButton(buttonsPrest, 3,  '3');

  debounceButton(buttonsPrest, 4,  '4');
  debounceButton(buttonsPrest, 5,  '5');
  debounceButton(buttonsPrest, 6,  '6');

  debounceButton(buttonsPrest, 7,  '7');
  debounceButton(buttonsPrest, 8,  '8');
  debounceButton(buttonsPrest, 9,  '9');

  if(debounceButton(buttonsPrest, 10, '*'))
  {
    inputBufferIndex--;
    inputBuffer[inputBufferIndex] = ' ';
  }
  debounceButton(buttonsPrest, 0,  '0');
  if(debounceButton(buttonsPrest, 11, '#'))
  {
    memset(inputBuffer, 0, sizeof(inputBuffer));
    inputBufferIndex = 0;
  }

  

}
/**
 * @brief Debounce een individuele knop en update het inputBuffer bij een stabiele druk.
 *
 * Deze functie detecteert veranderingen van een specifieke knop (bit 'i') in een matrix-keypad
 * of meerdere knoppen, past software-debounce toe, en plaatst het corresponderende karakter
 * 'c' in het inputBuffer bij een stabiele knopdruk (rising edge).
 *
 * @param buttonsPrest Huidige raw status van alle knoppen (bitmask).
 *                     Bit 'i' is de status van de knop die wordt gecheckt.
 *                     1 = knop ingedrukt, 0 = knop losgelaten.
 * @param i           Het bitnummer van de knop die gecontroleerd wordt (0-15).
 * @param c           Het karakter dat toegevoegd moet worden aan inputBuffer
 *                    wanneer deze knop stabiel wordt ingedrukt.
 *
 * @return bool       Geeft true terug als de knop stabiel is ingedrukt en het karakter
 *                    toegevoegd is aan inputBuffer (rising edge).
 *                    Geeft false terug als er geen stabiele druk detecteerd is.
 *
 * @note
 * - Debounce tijd is ingesteld via DEBOUNCE_TIME (ms).
 * - De functie houdt interne static variabelen bij:
 *      - stableState: huidige stabiele staat van alle knoppen
 *      - lastRawState: laatste ruwe status per bit
 *      - lastChangeTime: timestamp van laatste verandering per bit
 *      - lastButtonsPrest: stabiele toestand van alle knoppen
 * - inputBufferIndex moet buiten de functie worden beheerd om overflow te voorkomen.
 * - Deze functie moet regelmatig (bijv. in loop()) worden aangeroepen voor correcte debounce.
 * - CHAR_IN_STRING is de maximale grootte van inputBuffer om overflow te voorkomen.
 */
bool debounceButton(uint16_t buttonsPrest, uint16_t i, char c)
{
  static uint16_t stableState = 0;
  static uint16_t lastRawState = 0;
  static uint32_t lastChangeTime[16];  // per knop
  static uint16_t lastButtonsPrest =0;

  uint32_t now = millis();
  uint16_t mask = (1 << i);
  if((lastButtonsPrest & mask) != (buttonsPrest & mask))
  {
    if((buttonsPrest & mask) != (lastRawState & mask))
    {
      if(buttonsPrest & mask) lastRawState |= mask;
      else lastRawState &= ~mask;
      lastChangeTime[i] = now;
    }
    if(now - lastChangeTime[i] > DEBOUNCE_TIME)
    {
      if(buttonsPrest & mask)
      {
        lastButtonsPrest |= mask;
        inputBuffer[inputBufferIndex] = c;
        if(inputBufferIndex < CHAR_IN_STRING)
        {
          inputBufferIndex++;
        }
        return true;
      }
      else 
      {
        lastButtonsPrest &= ~mask;

      }
    }
  }
  return false;
}

bool getPinState(uint8_t pin)
{
  return digitalRead(pin) == LOW;
}
