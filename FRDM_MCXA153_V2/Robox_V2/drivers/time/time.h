#ifndef TIME_H
#define TIME_H

#include "board.h"
#include <stdint.h>

#define SEC_TO_MS 1000
#define MIN_TO_MS (60 * SEC_TO_MS)
/**
 * @brief Returns the number of milliseconds since the timer started.
 *
 * @return uint32_t Milliseconds elapsed since start.
 */
extern uint32_t ms;
static inline uint32_t millis(void)
{
    return ms;
}

/**
 * @brief Returns the number of microseconds since the timer started.
 *
 * @return uint32_t Microseconds elapsed since start.
 *
 * WARN This function has not been fully tested yet.
 */
static inline uint32_t micros(void)
{
    return DWT->CYCCNT / 48;
}

void millis_init(void);

#endif