#include "key.h"

volatile bool keyPinStatus;

void key_init(void)
{
    pin_init(GPIO3,  KEY_SENSOR_PIN, INPUT_PULUP, 0);
    GPIO3->ICR[KEY_SENSOR_PIN] |= PORT_PCR_IBE(1) | GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1011);


    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);

    keyPinStatus = getPinState(GPIO3, KEY_SENSOR_PIN);
}