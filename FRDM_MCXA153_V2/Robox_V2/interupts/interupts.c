#include "gpio.h"
#include "switch.c"
#include "key.c"

void GPIO3_IRQHandler(void)
{
    switchPinStatus = getPinState(GPIO3, SWITCH_SENSOR_PIN);
    keyPinStatus = getPinState(GPIO3, KEY_SENSOR_PIN);
}