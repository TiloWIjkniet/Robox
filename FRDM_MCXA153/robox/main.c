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
#include "hexDisplay.h"
#include "lpuart2.h"
int main(void)
{

  serial_init(115200);
  lpuart2_init(115200);
  printf("Starting game...\n");
    
  FSM_config();
  keyPad_init();
  millis_init();
  buzzer_init();
  touchSensor_init();
  hexDisplay_init();


  while(1)
  {
  
    FSM_runStateMachine();
    buzzer_loop();
    if(gameActiv)
    {
      touchUpdate();
      updateTimeGamePanaltuMillis();
      updateGameTimer();

    }

  }
}

