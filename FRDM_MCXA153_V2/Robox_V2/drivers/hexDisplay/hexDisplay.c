#include "gpio.h"

#define TM1637_CMD_DATA 0x40
#define TM1637_CMD_ADDR 0xC0
#define TM1637_CMD_DISP 0x8F

#define PIN_CLK 30
#define PIN_DIO 31

#define HEX_CHAR_LEN 11

#define OFF 10

static const uint8_t segments[HEX_CHAR_LEN] = 
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
};

void hexDisplay_init(void)
{
    pin_init(GPIO3, PIN_CLK, OUTPUT, 0);
    pin_init(GPIO3, PIN_DIO, OUTPUT, 0);
}

static inline void hexDisplay_setCLK(bool value)
{
    setPinState(GPIO3, PIN_CLK, value);
}

static inline void hexDisplay_setDIO(bool value)
{
    setPinState(GPIO3, PIN_DIO, value);
}

static inline void start(void) 
{
  hexDisplay_setCLK(true);
  hexDisplay_setDIO(true);
  hexDisplay_setDIO(false);
}

static inline void stop(void) 
{
  hexDisplay_setCLK(false);
  hexDisplay_setDIO(false);
  hexDisplay_setCLK(true);
  hexDisplay_setDIO(true);
}

void writeByte(uint8_t b) 
{
  for (int i = 0; i < 8; i++) 
  {
    hexDisplay_setCLK(false);

    if (b & 0x01) hexDisplay_setDIO(true);
    else          hexDisplay_setDIO(false);

    b >>= 1;

    hexDisplay_setCLK(true);
  }

  hexDisplay_setCLK(false);
  hexDisplay_setCLK(true);
}

void hexDisplay_digitsValues(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot) 
{
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



void hexDisplay_setTime(uint8_t min, uint8_t sec)
{
    const uint8_t dot = 1;
    uint8_t hex0 = min / 10;
    uint8_t hex1 = min % 10;
    uint8_t hex2 = sec / 10;
    uint8_t hex3 = sec % 10;
    hexDisplay_digitsValues(hex0, hex1, hex2, hex3, dot);
}

void hexDisplay_off(void)
{
    hexDisplay_digitsValues(OFF, OFF, OFF, OFF, OFF);
}

void hexDisplay_displayValue(uint8_t value)
{
    const uint8_t dot = 1;
    uint8_t hex0 = (value / 1000) % 10;
    uint8_t hex1 = (value / 100) % 10;
    uint8_t hex2 = (value / 10) % 10;
    uint8_t hex3 = value % 10;
    hexDisplay_digitsValues(hex0, hex1, hex2, hex3, dot);

}

bool hexDisplay_updateTimeOnChange(uint32_t ms)
{
  static lastSec = 0;
  uint16_t min = (ms / 1000) / (60);
  uint16_t sec = (ms / 1000) % (60);

  if(lastSec == sec) return false;
  
  hexDisplay_digitsValues(min,sec);
  return true;

}


