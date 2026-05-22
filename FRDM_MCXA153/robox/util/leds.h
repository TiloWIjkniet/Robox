#ifndef LEDS_H
#define LEDS_H

#include <MCXA153.h>

void leds_init(void);
void leds_shift(uint8_t rows, uint16_t cols);
void matrix_update(void);
void setLed(int row, int col);
void clearDisplay(void);
#endif