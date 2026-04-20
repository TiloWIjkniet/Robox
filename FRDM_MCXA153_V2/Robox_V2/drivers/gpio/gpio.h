#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

typedef struct
{
    bool lastRaw;
    bool lastStable;
    uint32_t lastChangeTime;
} debounce_t;

typedef enum 
{
    INPUT,
    OUTPUT,
    INPUT_INTERNAL_PULLUP
}pinMode_t;

/**
 * @brief Sets the output state of a GPIO pin.
 *
 * @param pGpio Pointer to the GPIO pGpio.
 * @param pin  Pin number within the pGpio.
 * @param value true = HIGH, false = LOW.
 */
static inline void setPinState(GPIO_Type *pGpio, const uint8_t pin, const bool value)
{
    pGpio->PSOR = (value << pin);
    pGpio->PCOR = ((!value) << pin); 
}


/**
 * @brief Reads the current state of a GPIO pin.
 * *
 * @param pGpio Pointer to the GPIO port.
 * @param pin  Pin number within the port.
 * @return true if pin is HIGH, false if LOW.
 */
static inline bool getPinState(const GPIO_Type *pGpio, const uint8_t pin)
{
    return (pGpio->PDIR & (1<<pin)) != 0U;
}

/**
 * @brief Initializes a GPIO pin.
 * *
 * @param pGpio   Pointer to the PORT peripheral.
 * @param pin     Pin number within the port.
 * @param pinMode Desired pin mode (INPUT, OUTPUT, INPUT_INTERNAL_PULLUP).
 * @param mux     Pin multiplexer setting (selects GPIO or peripheral function).
 */
void pin_init(GPIO_Type *pGpio, const uint8_t pin ,const pinMode_t pinMode ,const uint8_t mux);

/**
 * @brief Debounces a digital input signal.
 *
 *
 * @param pinStatus Current raw state of the input pin.
 * @param debounce  Pointer to debounce state structure.
 *
 * @return true  The current debounced (stable) state is HIGH.
 * @return false The current debounced (stable) state is LOW.
 *
 */
bool pin_debounce(const bool pinStatus, debounce_t *debounce);

#endif