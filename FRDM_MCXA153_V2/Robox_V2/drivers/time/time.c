
#include "time.h"
volatile uint32_t ms = 0;

void millis_init(void)
{
    SysTick_Config(48000);
    __enable_irq();
}
void SysTick_Handler(void)
{
    ms++;
}

