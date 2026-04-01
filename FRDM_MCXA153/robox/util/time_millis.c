#include "board.h"

volatile uint32_t ms = 0;

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

