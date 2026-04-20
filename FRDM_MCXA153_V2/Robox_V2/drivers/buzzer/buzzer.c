#include "buzzer.h"
#include "gpio.h"
#include "time.h"

#define BUZZER_PLAY_TIME 50
#define BUZZER_PIN 7


static bool lastStatus = false;
static uint32_t startBuzzer = 0;

void buzzer_init(void)
{
    pin_init(GPIO2, BUZZER_PIN, OUTPUT, 0);
}

void buzzer_setPin(bool val)
{
    setPinState(GPIO2, BUZZER_PIN, val);

    if(val) startBuzzer = millis();
    lastStatus = val;
}

bool buzzer_toggle(void)
{
    lastStatus = !lastStatus;
    buzzer_setPin(lastStatus);
    return lastStatus;
}

void buzzer_update(void)
{
    if(!lastStatus) return; //Buzzer off
    
    uint32_t now = millis();
    if(now - startBuzzer < BUZZER_PLAY_TIME) return;

    buzzer_setPin(false);
    
}