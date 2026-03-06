#include "board.h"
#include "fsm.h"
#include "serial.h"
#include "game_logic.h"
#include "time_millis.h"
#include "keypad.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "buzzer.h"
#include "touch_sensor.h"

int main(void)
{

  serial_init(115200);
  FSM_config();
  keyPad_init();
  millis_init();
  buzzer_init();
  touchSensor_init();


  while(1)
  {
  
    FSM_runStateMachine();
    if(gameActiv)
    {
      touchUpdate();
      updateTimeGamePanaltuMillis();
      updateGameTimer();
      buzzer_loop();
    }

  }
}

