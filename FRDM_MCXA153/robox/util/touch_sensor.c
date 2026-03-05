#include "time_millis.h"
#include "board.h"
#include <stdint.h>
#include <stdbool.h>

#define TOUCH_SENSOR_PIN 0
#define G_PIN 10
#define R_PIN 12
#define B_PIN 13
#define TOUCH_GLITCH_FILTER_MS  500
#define TOUCH_HOLD_TIME_MS  2000

typedef enum 
{
  OFF,
  WHITE,
  RED,
  GREEN
}collors_t;

void setCollor(collors_t collor);
bool getPinState_touchSensor(uint8_t pin);

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


    setCollor(WHITE);
}

bool isTouchLongPressed() 
{
  static bool isPressing = false;
  static uint32_t pressStart = 0;
  static uint32_t lastValidPress = 0;

  bool state = getPinState_touchSensor(TOUCH_SENSOR_PIN);
  uint32_t now = millis();

  if(state)
  {
    if(!isPressing)
    {
      pressStart = now;
      isPressing = true;
      setCollor(RED);
    }
    lastValidPress = now;
  }
  else if(isPressing)
  {
    if(now - lastValidPress > TOUCH_GLITCH_FILTER_MS)
    {
      isPressing = false;
      setCollor(WHITE);
    }
  }
  if(!isPressing) return false;

  if(now - pressStart >= TOUCH_HOLD_TIME_MS)
  {
    setCollor(GREEN);
    return true;
  }
  return false;
}

bool getPinState_touchSensor(uint8_t pin)
{
  return !((GPIO2->PDIR & (1<<pin)) == 0);
}

void pin_set(uint8_t pin, bool state)
{
    if(!state)GPIO1->PCOR = (1<<pin);
    else GPIO1->PSOR = (1<<pin);
}

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
