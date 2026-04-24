#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

typedef enum 
{
  OFF,
  WHITE,
  RED,
  GREEN
}collors_t;

void touchSensor_init(void);

void setCollor(collors_t collor);
bool isTouchPressed(void);
void setMustTouchSensor(bool must);
#endif