#ifndef SWITCH_AND_KEY_SENSORS_H
#define SWITCH_AND_KEY_SENSORS_H

#include <stdbool.h>
#include <stdint.h>

void switch_and_key_sensors_init();


bool readKeySensor();
bool readSwitchSensor();
bool readRawKeySensor();
bool readRawSwitchSensor();

#endif
