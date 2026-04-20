#ifndef FINGERPRINT_SCANNER_H
#define FINGERPRINT_SCANNER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {FINGERPRINT_OFF, FINGERPRINT_START, FINGERPRINT_SCANNING, FINGERPRINT_SUCCESS, FINGERPRINT_COMPLETE, FINGERPRINT_RESET} fingerprintState_t;

/**
 * @brief Initializes the fingerprint scanner hardware.
 */
void fingerprintScanner_init(void);


/**
 * @brief Updates the fingerprint scanner state machine.
 *
 * It must be called repeatedly in the main loop or periodic task.
 *
 * @param active
 *        If true, enables normal active scanning behavior and LED feedback.
 *        If false, the scanner operates in inactive/idle visual mode.
 *
 * @return Current fingerprint scanner state.
 */
fingerprintState_t fingerprintScanner_update(bool active);
#endif