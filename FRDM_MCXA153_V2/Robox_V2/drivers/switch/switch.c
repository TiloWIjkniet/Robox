#include "switch.h"

volatile bool switchPinStatus = false;

void switch_init(void)
{
    pin_init(GPIO3, SWITCH_SENSOR_PIN, INPUT_PULUP, 0);
    GPIO3->ICR[SWITCH_SENSOR_PIN] |= PORT_PCR_IBE(1) | GPIO_ICR_ISF(1) | GPIO_ICR_IRQC(0b1011);


    NVIC_SetPriority(GPIO3_IRQn, 3);
    NVIC_ClearPendingIRQ(GPIO3_IRQn);
    NVIC_EnableIRQ(GPIO3_IRQn);

    switchPinStatus = getPinState(GPIO3, SWITCH_SENSOR_PIN);
}

