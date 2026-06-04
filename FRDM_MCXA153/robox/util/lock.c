#include "lock.h"
#include "board.h"
#include "time_millis.h"
#include "game_logic.h"

#define LOCK_PIN 4
#define LOCK_DATA_PIN 5
#define LOCK_OPEN_TIME_MS 700

uint32_t lockOpenTime = 0;

void lock_init(void)
{
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);


    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);

    PORT2->PCR[LOCK_PIN] = PORT_PCR_LK(1);

    GPIO2->PDOR |= (1<<LOCK_PIN);
    GPIO2->PDDR |= (1<<LOCK_PIN);
    GPIO2->PCOR =  (1<<LOCK_PIN);

    PORT2->PCR[LOCK_DATA_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1);

}

void openLock(void)
{
    setPinState(GPIO2, LOCK_PIN, true);
    lockOpenTime = millis();
    
}

void closeLock()
{
    setPinState(GPIO2, LOCK_PIN, false);
    lockOpenTime = 0;
}

void lockUpdate(void)
{
    if(lockOpenTime != 0 && millis() - lockOpenTime > LOCK_OPEN_TIME_MS)
    {
       closeLock();
    }
}

bool isLockOpen()
{
    return !getPinState(GPIO2, LOCK_DATA_PIN);
}