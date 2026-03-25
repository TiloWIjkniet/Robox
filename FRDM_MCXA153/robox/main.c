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
#include "lpuart1.h"
#include "lpuart2.h"
#include "HM10.h"
int main(void)
{


  serial_init(115200);
  lpuart1_init(115200);
  // lpuart2_init word in void HM10_init() ingesteld 
  printf("Starting game...\n");
    
  FSM_config();
  keyPad_init();
  millis_init();
  buzzer_init();
  touchSensor_init();
  hexDisplay_init();
  HM10_init();
  
  receive_room_settings_from_esp();
  
  while(1)
  {
  
    FSM_runStateMachine();
    buzzer_loop();
    if(gameActiv)
    {
      updateHM10();
      touchUpdate();
      updateTimeGamePanaltuMillis();
      updateGameTimer();
      if(globalSettings.difficulty == WRONG_ANSWER_TIME_X2) updateVirtualTime();


    }

  }
}

