#ifndef TIME_MILLIS_H
#define TIME_MILLIS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Geeft het aantal milliseconden sinds het starten van de timer.
 *
 * @return uint32_t Aantal milliseconden sinds start.
 */
extern volatile uint32_t ms;
static inline uint32_t millis(void)
{
    return ms;
}

/**
 * @brief Initialiseert de milliseconde timer met behulp van de SysTick.
 */
void millis_init(void);

#endif