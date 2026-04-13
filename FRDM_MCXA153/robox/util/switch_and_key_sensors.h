#ifndef SWITCH_AND_KEY_SENSORS_H
#define SWITCH_AND_KEY_SENSORS_H

#include <stdbool.h>
#include <stdint.h>

void switch_and_key_sensors_init(void);


bool readKeySensor(void);
bool readSwitchSensor(void);
bool readRawKeySensor(void);
bool readRawSwitchSensor(void);

#endif
