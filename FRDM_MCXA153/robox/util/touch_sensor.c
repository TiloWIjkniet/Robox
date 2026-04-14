#include "time_millis.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>
#include "touch_sensor.h"
#include "game_logic.h"
#include "audio.h"

#define TOUCH_SENSOR_PIN 14
#define G_PIN 0
#define R_PIN 12
#define B_PIN 13
#define TOUCH_GLITCH_FILTER_MS  500
#define TOUCH_HOLD_TIME_MS  4000
#define FAST_BLINK_TIME 1000
#define MAX_BLINK_DELAY 200
#define MIN_BLINK_DELAY 50


void setCollor(collors_t collor);
bool getPinState_touchSensor(uint8_t pin);

/**
 * @brief Initialiseert de touch sensor en RGB LED hardware.
 */
void touchSensor_init(void)
{


    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_PORT3(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO3(1);

    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_PORT3(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO3(1);

    PORT3->PCR[G_PIN] = PORT_PCR_LK(1);
    PORT3->PCR[R_PIN] = PORT_PCR_LK(1);
    PORT3->PCR[B_PIN] = PORT_PCR_LK(1);
    GPIO3->PDOR |= (1<<G_PIN) |  (1<<R_PIN) | (1<<B_PIN);
    GPIO3->PDDR |= (1<<G_PIN) |  (1<<R_PIN) | (1<<B_PIN);
    GPIO3->PCOR =  (1<<G_PIN) |  (1<<R_PIN) | (1<<B_PIN);



    PORT3->PCR[TOUCH_SENSOR_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);


    setCollor(OFF);
}

bool mustPres = false;

bool isTouchPressed(void)
{
  typedef enum {RESET, FIRST_PRES, PRESSING, RELEASE, FIRST_LONG_PRESSED, LONG_PRESSED} touchState_t;
  static touchState_t touchState = RELEASE;
  static uint32_t pressStart = 0;
  static uint32_t lastValidPress = 0;
  static uint32_t blinkTime = 0;
  static bool blinkeState = false;

  bool isPressing = getPinState(GPIO3,TOUCH_SENSOR_PIN);
 


  uint32_t now = millis();
  if(isPressing)
  {
    lastValidPress = now;
  }
  switch (touchState)
  {
  case RESET:
  {
    stopAudio();
    touchState = RELEASE;
    break;
  }
  case RELEASE:
  {
    if(isPressing) 
    {
      touchState = FIRST_PRES;
    }
    setCollor(mustPres? WHITE : OFF);
    break;
  }
  case FIRST_PRES:
  {
    touchState = PRESSING;
    pressStart = now;
    setCollor(mustPres? GREEN : RED);
    playGlobelAudio(AUDIO_TOUCH_SENSOR_RUNNING);
    break;
  }
  case PRESSING:
  {
    if(now - lastValidPress > TOUCH_GLITCH_FILTER_MS) touchState = RESET;
    if(now - pressStart > TOUCH_HOLD_TIME_MS) touchState = FIRST_LONG_PRESSED;

    float ratio = (float)(now - pressStart) / (float)(TOUCH_HOLD_TIME_MS - FAST_BLINK_TIME);
    if (ratio > 1.0f) ratio = 1.0f;
    uint32_t blinkDelay = MAX_BLINK_DELAY - (MAX_BLINK_DELAY - MIN_BLINK_DELAY) * (ratio * ratio);
    if(now - blinkTime > blinkDelay)
    {
      blinkTime = now;
      blinkeState = !blinkeState;
      if(mustPres) setCollor(blinkeState? WHITE : GREEN);
      else         setCollor(blinkeState? OFF   : RED);
      
    }
    break;
  }
  case FIRST_LONG_PRESSED:
  {
    setCollor(mustPres? GREEN : RED);
    stopAudio();
    playGlobelAudio(mustPres ? AUDIO_TOUCH_SENSOR_CORRECT: AUDIO_TOUCH_SENSOR_WRONG);
    touchState = LONG_PRESSED;
     
    break;
  }
  case LONG_PRESSED:
  {
    if(now - lastValidPress > TOUCH_GLITCH_FILTER_MS) touchState = RESET;
    mustPres = false;
    return true;
    break;
  }
  }
  return false;

}

/**
 * @brief Stelt in of een "must press" toestand verplicht is voor de touch sensor.
 * 
 * @param must true om aanraking verplicht te maken, false om niet verplicht te maken
 */
void setMustTouchSensor(bool must)
{
  mustPres = must;
}


/**
 * @brief Zet de RGB LED op een bepaalde kleur.
 *
 * Deze functie schakelt de RGB LED pins aan of uit afhankelijk
 * van de gekozen kleur uit de collors_t enumeratie.
 *
 * Ondersteunde kleuren:
 * - OFF   : LED uit
 * - WHITE : R + G + B
 * - RED   : alleen rood
 * - GREEN : alleen groen
 *
 * @param collor gewenste LED kleur
 */
void setCollor(collors_t collor)
{
  bool r = false;
  bool g = false;
  bool b = false;
  switch (collor) 
  {
    case OFF:

    break;
    case WHITE:
      g = true;
      r = true;
      b = true;
    break;
    case RED:
      r = true;
    break;
    case GREEN:
      g = true;
    break;
    default: 

    break;
  }
  setPinState(GPIO3, R_PIN, r);
  setPinState(GPIO3, G_PIN, g);
  setPinState(GPIO3, B_PIN, b);
}
