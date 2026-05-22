#include "leds.h"
#include "fsl_device_registers.h"
#include "time_millis.h"
#include <stdint.h>
#include <stdbool.h>
#include "board.h"

#define DATA_PIN    27
#define CLOCK_PIN  11
#define LATCH_PIN   0

//WARN: OE_PINT is niet een meschkbare pin dus alles uit gekomt 
//#define OE_PIN     (1 << 3)
//NOTE: OE_PIN Moet wel altijd laag zijn, dus deze wordt niet gebruikt in de code maar moet wel fysiek laag worden gehouden


#define ROWS 8
#define COLS 16

uint8_t display[ROWS][COLS] =
{
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};


void clearDisplay(void)
{
    for(int row = 0; row < ROWS; row++)
    {
        for(int col = 0; col < COLS; col++)
        {
            display[row][col] = 0;
        }
    }
}

void setLed(int row, int col)
{
    if(row < 0 || row >= ROWS || col < 0 || col >= COLS) return;
    display[row][col] =1;
}

void matrix_update(void)
{
    static uint8_t row = 0;
    static uint32_t lastTime = 0;

    if (millis() - lastTime < 2) return;
    lastTime = millis();

    uint8_t currentRow = row;
    row++;
    if(row >= ROWS) row = 0;

    uint16_t colBits = 0;

    for(int col = 0; col < COLS; col++)
    {
        if(display[currentRow][col])
            colBits |= (1 << col);
    }

    uint8_t rowBits = (1 << currentRow);

    leds_shift(rowBits, colBits);
}


void leds_init(void)
{

    MRCC0->MRCC_GLB_CC1  |= MRCC_MRCC_GLB_CC1_GPIO1(1);
    MRCC0->MRCC_GLB_CC0  |= MRCC_MRCC_GLB_CC0_PORT1(1);
    
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO1(1);
    MRCC0->MRCC_GLB_RST0 |= MRCC_MRCC_GLB_RST0_PORT1(1);


    MRCC0->MRCC_GLB_CC1  |= MRCC_MRCC_GLB_CC1_GPIO2(1);
    MRCC0->MRCC_GLB_CC0  |= MRCC_MRCC_GLB_CC0_PORT2(1);
    
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO2(1);
    MRCC0->MRCC_GLB_RST0 |= MRCC_MRCC_GLB_RST0_PORT2(1);

    MRCC0->MRCC_GLB_CC1  |= MRCC_MRCC_GLB_CC1_GPIO3(1);
    MRCC0->MRCC_GLB_CC1  |= MRCC_MRCC_GLB_CC1_PORT3(1);
    
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO3(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_PORT3(1);

    PORT3->PCR[DATA_PIN] = 0x00008000; // DATA
    PORT1->PCR[CLOCK_PIN] = 0x00008000; // CLOCK
    PORT2->PCR[LATCH_PIN]  = 0x00008000; // LATCH
    //PORT2->PCR[OE_PIN]  = 0x00008000; // OE

    GPIO3->PDDR |= (1 << DATA_PIN);//| (1 << OE_PIN);
    GPIO1->PDDR |= (1 << CLOCK_PIN);
    GPIO2->PDDR |= (1 << LATCH_PIN);

    GPIO3->PCOR = (1 << DATA_PIN) ;//| (1 << OE_PIN);
    GPIO1->PCOR = (1 << CLOCK_PIN);
    GPIO2->PCOR = (1 << LATCH_PIN);

    clearDisplay();
}

void leds_shift(uint8_t rows, uint16_t cols)
{
    //GPIO3->PSOR = (1 << OE_PIN);
    GPIO2->PCOR = (1 << LATCH_PIN);

    uint8_t rowBits = ~rows;

    for(int i = 7; i >= 0; i--)
    {
        if(rowBits & (1 << i))
            GPIO3->PSOR = (1 << DATA_PIN);
        else
            GPIO3->PCOR = (1 << DATA_PIN);

        GPIO1->PSOR = (1 << CLOCK_PIN);
        GPIO1->PCOR = (1 << CLOCK_PIN);
    }

    for(int i = 15; i >= 0; i--)
    {
        if(cols & (1 << i))
            GPIO3->PSOR = (1 << DATA_PIN);
        else
            GPIO3->PCOR = (1 << DATA_PIN);

        GPIO1->PSOR = (1 << CLOCK_PIN);
        GPIO1->PCOR = (1 << CLOCK_PIN);
    }

    GPIO2->PSOR = (1 << LATCH_PIN);
    __NOP();
    __NOP();
    GPIO2->PCOR = (1 << LATCH_PIN);

    //GPIO3->PCOR = (1 << OE_PIN);
}