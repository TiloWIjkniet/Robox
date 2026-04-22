#ifndef SWITCH_H
#define SWITCH_H

#include "gpio.h"

#define SWITCH_SENSOR_PIN 1

void switch_init(void);

static inline bool switch_getRawState(void)
{
    return !getPinState(GPIO3, SWITCH_SENSOR_PIN);
}

static inline bool switch_getState(void)
{
    static debounce_t debounce;
    bool rawState = switch_getRawState();
    return !pin_debounce(rawState, &debounce, DEFAULT_DEBOUNCE_TIME);
}

#endif