#include "delay.h"
#include "time.h"

void delay(uint32_t ms)
{
    uint32_t startDelay = millis();
    while (millis() - startDelay < ms)
    {
        __NOP();
    }
}

void tinyDelay(uint32_t loops)
{
    for (uint32_t i = 0; i < loops; i++)
    {
        __NOP();
    }
}