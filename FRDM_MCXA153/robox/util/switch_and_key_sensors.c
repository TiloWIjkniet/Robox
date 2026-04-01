#include "time_millis.h"


#define KEY_SENSOR_PIN 0
#define SWITCH_SENSOR_PIN 1

#define BOUNCE_DELAY 50

void switch_and_key_sensors_init()
{
    // init key sensor pin
    //inti switch sensor pin
}

bool readRawKeySensor()
{
    // read key sensor pin
    return false;
}

bool readKeySensor()
{
    static bool lastKeyState = false;
    static bool debouncedKeyState = false;
    static uint32_t lastDebounceTime = 0;

    bool keyState = readRawKeySensor(); // read key sensor pin

    uint32_t now = millis();
    
    if(lastKeyState != keyState) lastDebounceTime = now;

    if(now - lastDebounceTime > BOUNCE_DELAY) 
    {
        debouncedKeyState = keyState;  
    }
    lastKeyState = keyState;
    return debouncedKeyState;
}

bool readRawSwitchSensor()
{
    // read switch sensor pin
    return false;
}

bool readSwitchSensor()
{
    static bool lastSwitchState = false;
    static bool debouncedSwitchState = false;
    static uint32_t lastDebounceTime = 0;

    bool switchState = readRawSwitchSensor(); // read switch sensor pin

    uint32_t now = millis();
    
    if(lastSwitchState != switchState) lastDebounceTime = now;

    if(now - lastDebounceTime > BOUNCE_DELAY) 
    {
        debouncedSwitchState = switchState;
    }
    lastSwitchState = switchState;
    return debouncedSwitchState;
}