#include "time_millis.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>
#include "touch_sensor.h"
#include "game_logic.h"

#define TOUCH_SENSOR_PIN 0
#define G_PIN 10
#define R_PIN 12
#define B_PIN 13
#define TOUCH_GLITCH_FILTER_MS  500
#define TOUCH_HOLD_TIME_MS  2000
#define BLINNK_DELAY 200


void setCollor(collors_t collor);
bool getPinState_touchSensor(uint8_t pin);

/**
 * @brief Initialiseert de touch sensor en RGB LED hardware.
 */
void touchSensor_init()
{
    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT1(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO1(1);

    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT1(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO1(1);

    MRCC0->MRCC_GLB_CC0_SET = MRCC_MRCC_GLB_CC0_PORT2(1);
    MRCC0->MRCC_GLB_CC1_SET = MRCC_MRCC_GLB_CC1_GPIO2(1);

    MRCC0->MRCC_GLB_RST0_SET = MRCC_MRCC_GLB_RST0_PORT2(1);
    MRCC0->MRCC_GLB_RST1_SET = MRCC_MRCC_GLB_RST1_GPIO2(1);

    PORT1->PCR[G_PIN] = PORT_PCR_LK(1);
    PORT1->PCR[R_PIN] = PORT_PCR_LK(1);
    PORT1->PCR[B_PIN] = PORT_PCR_LK(1);

    GPIO1->PDOR |= (1<<G_PIN) | (1<<R_PIN) | (1<<B_PIN);
    GPIO1->PDDR |= (1<<G_PIN) | (1<<R_PIN) | (1<<B_PIN);
    GPIO1->PCOR =  (1<<G_PIN) | (1<<R_PIN) | (1<<B_PIN);

    PORT2->PCR[TOUCH_SENSOR_PIN] = PORT_PCR_LK(1) | PORT_PCR_IBE(1);


    setCollor(OFF);
}

bool longPres = false;
bool mustPres = false;
void touchUpdate()
{
  static bool isPressing = false;
  static uint32_t pressStart = 0;
  static uint32_t lastValidPress = 0;
  static uint32_t blinkTime = 0;
  static bool letState = false; 
  static bool newPres = false;
  bool state = getPinState_touchSensor(TOUCH_SENSOR_PIN);

  uint32_t now = millis();

  if(state)
  {
    if(!isPressing)
    {
      isPressing = true;
      pressStart = now;
      newPres = true;
    }
    lastValidPress = now;
    if(now - blinkTime > BLINNK_DELAY && !longPres)
    {
      blinkTime = now;
      letState =!letState;
      setCollor(letState? (mustPres? GREEN : RED) : (mustPres? WHITE : OFF));
    }
  }
  else if(isPressing)
  {
    if(now - lastValidPress> TOUCH_GLITCH_FILTER_MS)
    {
      isPressing = false;
      setCollor(mustPres? WHITE : OFF);
      longPres = false;
      newPres = false;
    }
  }

  if(isPressing && now - pressStart > TOUCH_HOLD_TIME_MS)
  {

    if(newPres)
    {
      setCollor(mustPres? GREEN : RED);
      if(!mustPres) applyWrongAnswerPenalty();
      newPres = false;
    }

    longPres = true;
  }
}


/**
 * @brief Controleert of de touch sensor lang genoeg wordt ingedrukt.
 *
 * Deze functie detecteert een "long press" op de touch sensor.
 *
 * LED indicatie:
 * - Wit  : idle
 * - Rood : aanraking bezig
 * - Groen: long press gedetecteerd
 *
 * @return true  wanneer de sensor langer dan TOUCH_HOLD_TIME_MS wordt ingedrukt
 * @return false wanneer er geen geldige long press is
 */
bool isTouchLongPressed() 
{
  if(longPres) mustPres = false;
  else if(!mustPres) {mustPres = true; setCollor(WHITE);}
  return longPres;
}

/**
 * @brief Leest de huidige status van een GPIO pin voor de touch sensor.
 * *
 * @param pin GPIO pin nummer dat gelezen moet worden
 *
 * @return true  wanneer de pin HIGH is
 * @return false wanneer de pin LOW is
 */
bool getPinState_touchSensor(uint8_t pin)
{
  return !((GPIO2->PDIR & (1<<pin)) == 0);
}

/**
 * @brief Zet een GPIO pin hoog of laag.
 *
 * @param pin   GPIO pin nummer
 * @param state true  = HIGH
 *              false = LOW
 */
void pin_set(uint8_t pin, bool state)
{
    if(!state)GPIO1->PCOR = (1<<pin);
    else GPIO1->PSOR = (1<<pin);
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
  pin_set(R_PIN, r);
  pin_set(G_PIN, g);
  pin_set(B_PIN, b);
}
