#ifndef DELAY_H
#define DELAY_H

#include <stdint.h> 

/**
 * @brief Blocking delay in milliseconds.
 *
 * Waits until the specified number of milliseconds has passed
 * based on the system tick from millis().
 *
 * @note This is a blocking delay (CPU stays in loop).
 * @note Accuracy depends on millis() implementation and timer resolution.
 *
 * @param ms Number of milliseconds to wait.
 */
void delay(uint32_t ms);

/**
 * @brief Very short busy-wait delay.
 *
 * Executes a fixed number of no-operation (NOP) instructions.
 * Used for very small timing gaps where millisecond delay is too large.
 *
 * @note Not time-accurate (depends on CPU frequency and compiler optimization).
 * @note Blocking function.
 *
 * @param loops Number of NOP iterations.
 */
void tinyDelay(uint32_t loops);
#endif