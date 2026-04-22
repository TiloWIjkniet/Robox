#include "switch.h"

void switch_init(void)
{
    pin_init(GPIO3, SWITCH_SENSOR_PIN, INPUT_PULUP, 0);
}