#include "board.h"
#include "time_millis.h"
#include "game_data.h"

#define BUZZERT_PIN 7
#define BUZZER_DELAY 1.1
uint32_t durationMs = 0;
uint32_t startBuzzer = 0;
bool buzzerState = false;

void buzzer_init(void)
{

    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);

    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);

    PORT2->PCR[BUZZERT_PIN] = PORT_PCR_LK(1);

    GPIO2->PDOR |= (1<<BUZZERT_PIN) ;
    GPIO2->PDDR |= (1<<BUZZERT_PIN) ;

    GPIO2->PCOR = (1<<BUZZERT_PIN);
}

void buzzer_set(bool state)
{
    buzzerState = state;
    if(!state)GPIO2->PCOR = (1<<BUZZERT_PIN);
    else GPIO2->PSOR = (1<<BUZZERT_PIN);
}

void buzzer_play(uint32_t my_durationMs)
{
    if(buzzerState || globalSettings.audio == AUDIO_OFF)return;
    if(millis() - startBuzzer < durationMs * BUZZER_DELAY) return;

    buzzer_set(true);
    durationMs = my_durationMs;
    startBuzzer = millis();
}

void buzzer_loop()
{
    if(!buzzerState) return;
    if(millis() - startBuzzer >= durationMs)
    {
        buzzer_set(false);
    }
}