#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef enum 
{
    D_NON
} displayTemplate_t;

/**
 * @brief Checks whether a displayTemplate is already present in the queue.
 
 * @param displayTemplate The template to search for in the queue.
 *
 * @return true  If the template is found in the queue.
 * @return false If the template is not present in the queue.
 */
bool display_queueContains(const displayTemplate_t displayTemplate);

/**
 * @brief Updates the display queue
 *
 * Processes the item at the tail of the queue:
 * - If QUEUED: activates it and starts its timer.
 * - If ACTIVE: keeps it active until its duration expires.
 * - If expired: marks it DONE and advances the queue.
 *
 * @note Is intended to by uset in the main loop
 */
void display_updateQueue(void);

/**
 * @brief Forces a display template, clearing the current queue.
 * *
 * @param displayTemplate The template to display.
 * @param duration_ms     Duration to show the template (in milliseconds).
 *
 * @note
 * - The queue is fully reset (tail = 0, head = 1).


 */
int display_forceTemplate(const displayTemplate_t displayTemplate, const uint32_t duration_ms);

/**
 * @brief Adds a display template to the queue.
 *
 * Inserts a new template at the head of the queue if there is space
 * and the template is not already active or queued.
 *
 * @param displayTemplate The template to enqueue.
 * @param duration_ms     Duration to show the template (in milliseconds).
 * 
 */
int display_queueTemplate(const displayTemplate_t displayTemplate, const uint32_t duration_ms);

#define DISPLAY_ON_PC 1
#if DISPLAY_ON_PC
    #include <stdio.h>
#endif

#endif