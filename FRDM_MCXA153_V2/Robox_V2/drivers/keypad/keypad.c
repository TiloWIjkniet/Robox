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


#define DEFAULT_CHAR '\0'
#define MAX_INPUT_BUFFER_LEN (MAX_INPUT_DATA_LEN - 1)
const uint8_t keymap[ROWS][COLS] =
{
  {'3','2','1'},
  {'6','5','4'},
  {'9','8','7'},
  {'#','0','*'}
};

typedef struct
{
    debounce_t debounce;
    bool lastStatus;
}keypadButtons_t;

bool answerAvailable = false;
uint8_t inputBuffer_len = 0; 
char inputBuffer[MAX_INPUT_BUFFER_LEN];
inputData_t inputData;


void keypad_init(void)
{
    pin_init(GPIO3, PIN_COLUM_0, OUTPUT, 0);
    pin_init(GPIO3, PIN_COLUM_1, OUTPUT, 0);
    pin_init(GPIO3, PIN_COLUM_2, OUTPUT, 0);

    pin_init(GPIO3, PIN_ROW_0, INPUT_PULLUP, 0);
    pin_init(GPIO3, PIN_ROW_1, INPUT_PULLUP, 0);
    pin_init(GPIO3, PIN_ROW_2, INPUT_PULLUP, 0);
    pin_init(GPIO3, PIN_ROW_3, INPUT_PULLUP, 0);
}

bool keypad_answerAvailable(void)
{
    return answerAvailable;
}

void keypad_getAnswer(inputData_t answer)
{
   memcpy(answer, inputData, sizeof(inputData_t)); 
   answerAvailable = false;
}

char keypad_getkey(void)
{
    static keypadButtons_t keypadButtons[COLS][ROWS] = {};
    for (uint8_t col = 0; col < COLS; col++)
    {   
        setPinState(GPIO3, pin_colums[col], true);
    }


  for(uint8_t col = 0; col < COLS; col++)
  {
    setPinState(GPIO3, pin_colums[col], false);

    for (uint8_t row =0; row < ROWS; row++)
    {
        bool val = getPinState(GPIO3, pin_row[row]);
        bool debouncedVal = pin_debounce(val, &keypadButtons[col][row].debounce);

        bool prev = keypadButtons[col][row].lastStatus;
        keypadButtons[col][row].lastStatus = debouncedVal;

        if(debouncedVal && !prev) return keymap[row][col];
        
        
    }
    setPinState(GPIO3, pin_colums[col], true);

  }
  return DEFAULT_CHAR;
    
}

bool keypad_update(void)
{
    char key = keypad_getkey();
    switch (key) 
    {
        case '\0':
        {
            return false;
        }
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
            inputBuffer[inputBuffer_len] = key;
            if(inputBuffer_len < MAX_INPUT_BUFFER_LEN) inputBuffer_len ++;

            break;
        }
    }
    return true;
}

