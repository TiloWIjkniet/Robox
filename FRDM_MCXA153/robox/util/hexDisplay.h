#ifndef HEX_DISPLAY_H
#define HEX_DISPLAY_H

void hexDisplay_init();
void hexDisplay_setTime(uint8_t minutes, uint8_t seconds);
void displayDigits(const uint8_t d[5]);
void displayDigitsValues(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t dot);

#define OFF 10
#define ERROR_HEX_DIS 11
#define DEV_MODE_HEX_DIS 12
#define IDLE_MODE_HEX_DIS 13

extern const uint8_t IDLE_MODE_HEX[5];
extern const uint8_t ERROR_HEX[5];
extern const uint8_t DEV_MODE_HEX[5];
extern const uint8_t ALL_OFF_HEX[5];


#endif