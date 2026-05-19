#include "board.h"
#include <stdio.h>
#include <string.h>
#include "hexDisplay.h"
#include "game_logic.h"
#include "game_data.h"

#define TM1637_CMD_DATA 0x40
#define TM1637_CMD_ADDR 0xC0
#define TM1637_CMD_DISP 0x8F

#define PIN_CLK 30
#define PIN_DIO 29

#define NUMBER_OF_HEX_NUMBERS 14

const uint8_t IDLE_MODE_HEX[5] = {OFF, IDLE_MODE_HEX_DIS, OFF, IDLE_MODE_HEX_DIS, OFF};
const uint8_t ERROR_HEX[5] = {OFF, ERROR_HEX_DIS, OFF, ERROR_HEX_DIS, OFF};
const uint8_t DEV_MODE_HEX[5] = {OFF, DEV_MODE_HEX_DIS, OFF, DEV_MODE_HEX_DIS, OFF};
const uint8_t ALL_OFF_HEX[5] = {OFF, OFF, OFF, OFF, OFF};

void hexDisplay_init(void)
{
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    PORT3->PCR[PIN_CLK] = PORT_PCR_LK(1);
    PORT3->PCR[PIN_DIO] = PORT_PCR_LK(1);

    GPIO3->PDOR |= (1<<PIN_CLK) | (1<<PIN_DIO);
    GPIO3->PDDR |= (1<<PIN_CLK) | (1<<PIN_DIO);


}

void hexDisplay_setTime(uint8_t minutes, uint8_t seconds)
{
    uint8_t hex0 = minutes / 10;
    uint8_t hex1 = minutes % 10;
    uint8_t hex2 = seconds / 10;
    uint8_t hex3 = seconds % 10;

    char colom = 1;
    //printf("min:%d sec: %d\n",minutes, seconds);
    displayDigitsValues(hex0, hex1, hex2, hex3, colom);
}


void start(void) 
{
  setPinState(GPIO3, PIN_CLK, true);
  setPinState(GPIO3, PIN_DIO, true);
  setPinState(GPIO3, PIN_DIO, false);
}

void stop(void) 
{
  setPinState(GPIO3, PIN_CLK, false);
  setPinState(GPIO3, PIN_DIO, false);
  setPinState(GPIO3, PIN_CLK, true);
  setPinState(GPIO3, PIN_DIO, true);
}

void writeByte(uint8_t b) 
{
  for (int i = 0; i < 8; i++) 
  {
    setPinState(GPIO3, PIN_CLK, false);

    if (b & 0x01) setPinState(GPIO3, PIN_DIO, true);
    else          setPinState(GPIO3, PIN_DIO, false);

    b >>= 1;

    setPinState(GPIO3, PIN_CLK, true);
  }

  setPinState(GPIO3, PIN_CLK, false);
  setPinState(GPIO3, PIN_CLK, true);
  
}

void displayDigitsValues(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot) 
{
  const uint8_t segments[NUMBER_OF_HEX_NUMBERS] = 
  {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b00000000, // OFF
    0b01111001, // E
    0b01011110, // D
    0b00000110  // I
  };


  uint8_t seg0 = segments[d0];
  uint8_t seg1 = segments[d1];
  uint8_t seg2 = segments[d2];
  uint8_t seg3 = segments[d3];

  if (dot != OFF) seg1 |= 0b10000000;

  start();
  writeByte(TM1637_CMD_DATA);
  stop();

  start();
  writeByte(TM1637_CMD_ADDR);
  writeByte(seg0);
  writeByte(seg1);
  writeByte(seg2);
  writeByte(seg3);
  stop();

  start();
  writeByte(TM1637_CMD_DISP);
  stop();
}

void displayDigits(const uint8_t d[5])
{
    displayDigitsValues(d[0], d[1], d[2], d[3], d[4]);
} 