
#ifndef BUZZER_H
#define BUZZER_H
#include "time_millis.h"
#include <stdint.h>
#include <stdbool.h>

#define BUZZERT_DURATION 100 // Pas dit aan naar de juiste pin voor de buzzer
void buzzer_init(void);
void buzzer_play(uint32_t my_durationMs);
void buzzer_loop();

#endif