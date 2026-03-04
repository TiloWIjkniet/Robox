#include "board.h"

static volatile uint32_t ms = 0;

void millis_init()
{
    SysTick_Config(48000);
    __enable_irq();
}
void SysTick_Handler(void)
{
    ms++;
}

uint32_t millis()
{
    return ms;
}