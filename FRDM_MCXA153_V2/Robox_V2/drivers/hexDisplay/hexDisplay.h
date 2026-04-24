#ifndef HEX_DISPLAY_H
#define HEX_DISPLAY_H

#include <stdint.h>

/**
 * @brief Turns off the entire display.
 *
 * Clears all 4 digits by sending the OFF segment value.
 */
void hexDisplay_off(void);

/**
 * @brief Displays a 4-digit numeric value on the display.
 *
 * Converts a numeric value into individual digits and sends them
 * to the display driver.
 *
 * @param value Number to display (0–9999 recommended).
 *
 */
void hexDisplay_displayValue(uint8_t value);

void hexDisplay_setTime(uint8_t min, uint8_t sec);


bool hexDisplay_updateTimeOnChange(uint32_t ms);

#endif