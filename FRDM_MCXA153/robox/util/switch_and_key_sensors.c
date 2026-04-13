#include "time_millis.h"
#include "board.h"
#include "game_logic.h"
#define KEY_SENSOR_PIN 15
#define SWITCH_SENSOR_PIN 1

#define BOUNCE_DELAY 50

void switch_and_key_sensors_init(void)
{


    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    PORT3->PCR[KEY_SENSOR_PIN] =    PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
    PORT3->PCR[SWITCH_SENSOR_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
    // init key sensor pin
    //inti switch sensor pin
}

bool readRawKeySensor(void)
{
    // read key sensor pin
    return getPinState(GPIO3, KEY_SENSOR_PIN);
}

bool readKeySensor(void)
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

bool readRawSwitchSensor(void)
{
    // read switch sensor pin

    return getPinState(GPIO3, SWITCH_SENSOR_PIN);

}

bool readSwitchSensor(void)
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