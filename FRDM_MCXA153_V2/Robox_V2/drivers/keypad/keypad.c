#include "gpio.h"

#define ROWS 4
#define COLS 3

#define PIN_COLUM_0 10
#define PIN_COLUM_1 6
#define PIN_COLUM_2 8
const uint8_t pin_colums[COLS] = {PIN_COLUM_0, PIN_COLUM_1, PIN_COLUM_2};

#define PIN_ROW_0 7 // Controleren of goed
#define PIN_ROW_1 31
#define PIN_ROW_2 11
#define PIN_ROW_3 9
const uint8_t pin_row[ROWS] = {PIN_ROW_0, PIN_ROW_1, PIN_ROW_2, PIN_ROW_3};

#define NO_KEY 0
#define DEFAULT_CHAR '\0'

#define MAX_INPUT_BUFFER_LEN (MAX_INPUT_DATA_LEN - 1)
const uint8_t keymap[ROWS][COLS] =
{
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'},
  {'#','0','*'}
};

bool answerAvailable = false;
uint8_t inputBuffer_len = 0; 
char inputBuffer[MAX_INPUT_BUFFER_LEN];
inputData_t inputData;

void keypad_init(void)
{
    const uint8_t *rows = pin_row;
    const uint8_t *cols = pin_colums;

    for (uint8_t i = 0; i < ROWS; i++)
    {
        pin_init(GPIO3, rows[i], INPUT_PULLUP, 0);
    }

    for (uint8_t i = 0; i < COLS; i++)
    {   
        pin_init(GPIO3, cols[i], OUTPUT, 0);
        setPinState(GPIO3, cols[i], true);
    }
}

void keypad_getAnswer(inputData_t *answer)
{
   memcpy(answer, inputData, sizeof(inputData_t)); 
   answerAvailable = false;
}

char keypad_getkey(void)
{
    static debounce_t keypadButtons[COLS][ROWS] = {};

    const uint8_t *cols = pin_colums;
    const uint8_t *rows = pin_row;
  for(uint8_t col = 0; col < COLS; col++)
  {
    setPinState(GPIO3, cols[col], false);

    for (uint8_t row =0; row < ROWS; row++)
    {
        bool val = getPinState(GPIO3, rows[row]);
        bool prev = keypadButtons[col][row].lastStable;
        bool debouncedVal = pin_debounce(val, &keypadButtons[col][row], DEFAULT_DEBOUNCE_TIME);
        
        if(debouncedVal && !prev) 
        {
            setPinState(GPIO3, cols[col], true);
            return keymap[row][col];
        }
    }
    setPinState(GPIO3, cols[col], true);

  }
  return NO_KEY;
    
}

bool keypad_update(void)
{
    char key = keypad_getkey();
    if(key == NO_KEY) return;

    switch (key) 
    {
        case '#':
        {
            memcpy(inputData, inputBuffer, inputBuffer_len);
            inputData[inputBuffer_len] = '\0';

            answerAvailable = true;
            inputBuffer_len = 0;
            break;
        }
        case '*':
        {
            if(inputBuffer_len > 0 ) inputBuffer_len --;         
            break;
        }
        default:
        {
            
            if(inputBuffer_len < MAX_INPUT_BUFFER_LEN) 
            {
                inputBuffer[inputBuffer_len++] = key;
            }
            break;
        }
    }
    return true;
}

