#include "keu.h"

void key_init(void)
{
    pin_init(GPIO3,  KEY_SENSOR_PIN, INPUT_PULUP, 0);
}