#include "fingerprint_scanner.h"
#include "gpio.h"

#define FINGERPRINT_SCAN_TIME_MS 5000
#define FAST_BLINK_TIME 1000
#define MAX_BLINK_DELAY 200
#define MIN_BLINK_DELAY 50
#define BLINK_RANGE (MAX_BLINK_DELAY - MIN_BLINK_DELAY)

#define DEBOUNCE_TIME 100

#define TOUCH_SENSOR_PIN 14
#define G_PIN 0
#define R_PIN 12
#define B_PIN 13

typedef enum {OFF, WHITE, RED, GREEN} colors_t;

void fingerprintScanner_init(void)
{
    pin_init(GPIO3, TOUCH_SENSOR_PIN, INPUT, 0);

    pin_init(GPIO3, R_PIN, OUTPUT, 0);
    pin_init(GPIO3, G_PIN, OUTPUT, 0);
    pin_init(GPIO3, B_PIN, OUTPUT, 0);
}

fingerprintState_t fingerprintScanner_update(bool active)
{
    static fingerprintState_t state = FINGERPRINT_OFF;
    static uint32_t startScanTime = 0;
    static debounce_t debounce;

    static bool blinkState = false;
    static uint32_t blinkTime = 0;

    uint32_t now = millis();

    bool isPressing = getPinState(GPIO3, TOUCH_SENSOR_PIN);
    bool debouncedPress = pin_debounce(isPressing, &debounce, DEBOUNCE_TIME);

    switch (state)
    {
    case FINGERPRINT_OFF:
    {
        if (debouncedPress)
        {
            state = FINGERPRINT_START;
        }
        fingerprintScanner_setColor(active ? WHITE : OFF);
        return FINGERPRINT_OFF;
    break;
    }
    case FINGERPRINT_START:
    {
        state = FINGERPRINT_SCANNING;
        startScanTime = now;
        blinkState = false;
        blinkTime = now;
        fingerprintScanner_setColor(active ? GREEN : RED);
        return FINGERPRINT_START;
    }
    case FINGERPRINT_SCANNING:
    {
        if (!debouncedPress) 
        {
            state = FINGERPRINT_RESET;
            return FINGERPRINT_SCANNING;
        }
        else if(now - startScanTime > FINGERPRINT_SCAN_TIME_MS)
        {
            state = FINGERPRINT_SUCCESS;
            return FINGERPRINT_SCANNING;
        }
        else
        {
            uint32_t elapsed = now - startScanTime;
            if(elapsed > BLINK_RANGE) elapsed = BLINK_RANGE;

            uint32_t ratio = (elapsed * elapsed) / BLINK_RANGE;
            uint32_t blinkDelay = MAX_BLINK_DELAY - ((MAX_BLINK_DELAY - MIN_BLINK_DELAY) * ratio) / BLINK_RANGE;
            if(now - blinkTime >= blinkDelay)
            {
                blinkTime += blinkDelay;
                blinkState ^= 1;

                colors_t c1 = active ? WHITE : OFF;
                colors_t c2 = active ? GREEN : RED;
                fingerprintScanner_setColor(blinkState ? c1 : c2);
            }
        }


        return FINGERPRINT_SCANNING;
    }
    case FINGERPRINT_SUCCESS:
    {
        state = FINGERPRINT_COMPLETE;
        fingerprintScanner_setColor(active ? GREEN : RED);
        return FINGERPRINT_SUCCESS;
    }
    case FINGERPRINT_COMPLETE:
    {
         if (!debouncedPress) 
         {
            state = FINGERPRINT_RESET;
         }
        return FINGERPRINT_COMPLETE;
    }
    case FINGERPRINT_RESET:
    {
        fingerprintScanner_setColor(OFF);
        state = FINGERPRINT_OFF;
        return FINGERPRINT_RESET;
    }
    default:
    {
        state = FINGERPRINT_OFF;
        return FINGERPRINT_OFF;
    }
    }


}

static inline void fingerprintScanner_setColor(colors_t color)
{
    static const uint8_t colorTable[] = 
    {
        0b000, // OFF
        0b111, // WHITE
        0b100, // RED
        0b010  // GREEN
    };
    uint8_t c = colorTable[color];
    setPinState(GPIO3, R_PIN, (c & 0b100));
    setPinState(GPIO3, G_PIN, (c & 0b010));
    setPinState(GPIO3, B_PIN, (c & 0b001));
}