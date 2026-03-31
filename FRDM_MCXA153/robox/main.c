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
#include "audio.h"
int main(void)
{


  serial_init(9600);
  lpuart1_init(115200);
  //lpuart2_init word in void HM10_init() ingesteld 

  FSM_config();
  keyPad_init();
  millis_init();
  buzzer_init();
  touchSensor_init();
  hexDisplay_init();
  HM10_init();
  audio_init();
  receive_room_settings_from_esp();
  
  while(1)
  {
    updateHM10();
    FSM_runStateMachine();
    updateDisplayQueue();
    buzzer_loop();
    updateAudioQueue();
    if(gameActiv)
    {

      touchUpdate();
      updateTimeGamePenaltyMillis();
      updateGameTimer();
      if(globalSettings.difficulty == WRONG_ANSWER_TIME_X2) updateVirtualTime();
    }

  }
}

//WARN : De function voor gesenureert is to gevoegt maar grote kans dat er nog iets mis in zit
//WARN : audio module is toegevoegd, moet nog goed getest worden en geintergreert worden

//TODO: Taal van display templates wort nu aan gepast aan de hand van de audio instelling, hier voor een aparte instelling voor maken dat je hebt audio aan/ uit en taal NL/EN
//DONE: uitzoeken hoe lupart0 werkt.
//DONE: audio uitzoeken en triggers voor die aurio maken.

//DONE : In global setting bool maken voor gesensureert en uit audio halen. dit ook aanpassen in websiten en esp na aanlijding hier van de juiste template en audio evecten gebruiken.
//IDEA: In send run data rety function maken