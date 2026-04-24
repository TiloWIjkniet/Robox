#ifndef SWITCH_H
#define SWITCH_H

#include "gpio.h"

#define SWITCH_SENSOR_PIN 1

extern volatile bool switchPinStatus;
void switch_init(void);

static inline bool switch_getRawState(void)
{
    return !switchPinStatus;
}

static inline bool switch_getState(void)
{
    static debounce_t debounce;
    bool rawState = switch_getRawState();
    return !pin_debounce(rawState, &debounce, DEFAULT_DEBOUNCE_TIME);
}

#endif