#include "gpio.h"
#include "time.h"



static inline PORT_Type *getGpio(const GPIO_Type *pGpio)
{
    if(pGpio == GPIO0) return PORT0;
    if(pGpio == GPIO1) return PORT1;
    if(pGpio == GPIO2) return PORT2;
    if(pGpio == GPIO3) return PORT3;
    
}

static inline void enableClock(const GPIO_Type *pGpio)
{

    if(pGpio == GPIO0)
    {
        MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT0(1);
        MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO0(1);

        MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT0(1);
        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO0(1); 
        return;
    }
    if(pGpio == GPIO1)
    {
        MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);
        MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO1(1);

        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST0_PORT1(1);
        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO1(1); 
        return;
    }

    if(pGpio == GPIO2)
    {
        MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
        MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);

        MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);
        return;
    }
    if(pGpio == GPIO3)
    {
        MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
        MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
        MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1); 
        return;
    }

}


typedef struct
{
    bool lastRaw;
    uint32_t lastChangeTime;
} debounce_t;

bool pin_debounce(const bool pinStatus, debounce_t *debounce , const uint32_t bounce_time)
{
    uint32_t now = millis();
    if(pinStatus != debounce->lastRaw)
    {
        debounce->lastChangeTime = now;
        debounce->lastRaw = pinStatus;
        return false;
    }

    if(now - debounce->lastChangeTime < bounce_time )return false;

    return pinStatus;
}

void pin_init(GPIO_Type *pGpio, const uint8_t pin ,const pinMode_t pinMode ,const uint8_t mux)
{
    enableClock(pGpio);

    PORT_Type *pPort = getGpio(pGpio);
    switch (pinMode)
    {
    case INPUT:
    {
        pPort->PCR[pin] =   PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(mux);
        return;
    }
    case OUTPUT:
    {
        pPort->PCR[pin] =   PORT_PCR_LK(1) | PORT_PCR_MUX(mux);
        pGpio->PDOR    |=  (1U<<pin);
        pGpio->PDDR    |=  (1U<<pin);
        return;
    }
    case INPUT_INTERNAL_PULLUP:
    {
        pPort->PCR[pin] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_MUX(mux) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
        return;
    }
    default:
        return;
    }
}