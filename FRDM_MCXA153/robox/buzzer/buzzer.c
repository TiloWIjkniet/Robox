
#define BUZZERT_PIN 0

uint32_t durationMs = 0;
uint32_t startBuzzer = 0;
bool buzzerState = false;

void buzzer_init(void)
{

}

void buzzer_set(bool state)
{
    buzzerState = state;
    if(state)
    {

    }
    else
    {

    }
}

void buzzer_play(uint32_t my_durationMs)
{
    durationMs = my_durationMs;
    startBuzzer = millis();
    buzzer_set(true);
}

void buzzer_loop()
{
    if(buzzerState == false) return;
    
    if(millis() - startBuzzer >= durationMs)
    {
        buzzer_set(false);
    }
}