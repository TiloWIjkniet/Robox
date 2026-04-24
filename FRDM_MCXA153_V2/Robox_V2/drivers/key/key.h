#ifndef KEY_H
#define KEY_H

#include "gpio.h"

#define KEY_SENSOR_PIN 1

extern volatile bool keyPinStatus;

/**
 * @brief Initializes the switch hardware.
 *
 */
void key_init(void);



/**
 * @brief Reads the raw (non-debounced) key state.
 *
 * @return true if the key is pressed/active.
 * @return false if the key is not active.
 *
 * @note This reads the direct GPIO state without filtering noise.
 */
static inline bool key_getRawState(void)
{
    return !keyPinStatus;
}


/**
 * @brief Reads the debounced key state.
 *
 * Applies software debounce filtering to prevent false triggers
 * caused by mechanical key bouncing.
 *
 * @return true if the key is stably pressed.
 * @return false if the key is not active or bouncing.
 *
 */
static inline bool key_getState(void)
{
    static debounce_t debounce;
    bool rawState = key_getRawState();
    return !pin_debounce(rawState, &debounce, DEFAULT_DEBOUNCE_TIME);
}

#endif