#ifndef TOUCH_SENSOR_H
#define TOUCH_SENSOR_H

typedef enum 
{
  OFF,
  WHITE,
  RED,
  GREEN
}collors_t;

void touchSensor_init();
bool isTouchLongPressed(); 
void setCollor(collors_t collor);
void touchUpdate();
#endif