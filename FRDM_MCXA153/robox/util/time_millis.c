#include "board.h"

static volatile uint32_t ms = 0;

/**
 * @brief Initialiseert de milliseconde timer met behulp van de SysTick.
 */
void millis_init()
{
    SysTick_Config(48000);
    __enable_irq();
}
void SysTick_Handler(void)
{
    ms++;
}

/**
 * @brief Geeft het aantal milliseconden sinds het starten van de timer.
 *
 * @return uint32_t Aantal milliseconden sinds start.
 */
uint32_t millis()
{
    return ms;
}