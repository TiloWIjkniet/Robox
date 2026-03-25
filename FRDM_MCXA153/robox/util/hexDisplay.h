#ifndef HEX_DISPLAY_H
#define HEX_DISPLAY_H

void hexDisplay_init();
void hexDisplay_setTime(uint8_t minutes, uint8_t seconds);
void displayDigits(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot) ;
#define OFF 10
#endif