#include "board.h"

#define TM1637_CMD_DATA 0x40
#define TM1637_CMD_ADDR 0xC0
#define TM1637_CMD_DISP 0x8F

#define PIN_CLK 30
#define PIN_DIO 31


void displayDigits(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot);

void hexDisplay_init()
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
    displayDigits(hex0, hex1, hex2, hex3, colom);
}

void setPintate_hexDisplay(uint8_t pin, bool value)
{
  if(!value)GPIO3->PCOR = (1<<pin);
  else GPIO3->PSOR = (1<<pin);
}

void start(void) {
  setPintate_hexDisplay(PIN_CLK, true);
  setPintate_hexDisplay(PIN_DIO, true);
setPintate_hexDisplay(PIN_DIO, false);
}

void stop(void) {
  setPintate_hexDisplay(PIN_CLK, false);
  setPintate_hexDisplay(PIN_DIO, false);
  setPintate_hexDisplay(PIN_CLK, true);
  setPintate_hexDisplay(PIN_DIO, true);
}

void writeByte(uint8_t b) {
  for (int i = 0; i < 8; i++) {
    setPintate_hexDisplay(PIN_CLK, false);

    if (b & 0x01) setPintate_hexDisplay(PIN_DIO, true);
    else          setPintate_hexDisplay(PIN_DIO, false);

    b >>= 1;

    setPintate_hexDisplay(PIN_CLK, true);
  }

  setPintate_hexDisplay(PIN_CLK, false);
  setPintate_hexDisplay(PIN_CLK, true);
}

void displayDigits(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot) {
  uint8_t segments[] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111,
    0b01100110, 0b01101101, 0b01111101, 0b00000111,
    0b01111111, 0b01101111
  };

  uint8_t seg0 = segments[d0];
  uint8_t seg1 = segments[d1];
  uint8_t seg2 = segments[d2];
  uint8_t seg3 = segments[d3];

  if (dot) seg1 |= 0b10000000;

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