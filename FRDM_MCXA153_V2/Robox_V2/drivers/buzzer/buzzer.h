#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Sets the buzzer output pin state and updates internal state.
 *
 * This function directly controls the buzzer GPIO pin and updates the
 * internal state variables used for timing and logic control.
 *
 *
 * @param val Desired buzzer state:
 *            - true  = buzzer ON
 *            - false = buzzer OFF
 */
void buzzer_setPin(bool val)

/**
 * @brief Initializes the buzzer GPIO pin.
 *
 * Configures the buzzer pin as a digital output
 */
void buzzer_init(void);

/**
 * @brief Toggles the buzzer state.
 *
 * Switches the buzzer between ON and OFF states. 
 * Is auto reset in Buzzer_update() 
 *
 * @return true  If the buzzer is now ON.
 * @return false If the buzzer is now OFF.
 */
bool buzzer_toggle(void);


/**
 * @brief Updates buzzer timing and auto-stops it after a fixed duration.
 *
 * This function enforces a maximum buzzer ON time. If the buzzer has been
 * active longer than BUZZER_PLAY_TIME, it is automatically turned off.
 *
 * @note
 * - Must be called regularly (e.g. in main loop).
 */
void buzzer_update(void);

#endif