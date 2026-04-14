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
#include "switch_and_key_sensors.h"
#include "display.h"
int main(void)
{


  //serial_init(9600); //Word gezet in audio_init() 
 //lpuart1_init(115200); // word gebruik voor cominication naar esp
 // lpuart2_init //word in void HM10_init() ingesteld 


  //Initialisatie van alle modules
  //audio_init();
  st7920_init();
  // FSM_config();
  // keyPad_init();
   millis_init();
  // buzzer_init();
  // touchSensor_init();
  // hexDisplay_init();
  // HM10_init();
  
 // printf("Start game\n");
  //switch_and_key_sensors_init();

  //Voor dat programa kan starten
  //receive_room_settings_from_esp();

  st7920_writeb('h');
  st7920_writeb('e');
  st7920_writeb('e');

  while (1)
  {
    static uint32_t lastDisplayUpdate = 0;
    if(millis() - lastDisplayUpdate > 1) //update display elke 100 ms
    {
      lastDisplayUpdate = millis();
      st7920_update();
    }
  }
  
  while(1)
  {
    
    //Standaard updates die altijd moeten lopen
    FSM_runStateMachine();
    updateHM10();
    updateDisplayQueue();
    updateBuzzer();
    updateAudioQueue();
    updateInputBuffer();

    static uint32_t lastDisplayUpdate = 0;
    if(millis() - lastDisplayUpdate > 1) //update display elke 100 ms
    {
      lastDisplayUpdate = millis();
      st7920_update();
    }


    if(gameActiv)
    {
      //Updates die alleen moeten lopen als het spel actief is
      updateGameTimer();
      updateSpecialActies();
      updateTimeGamePenaltyMillis();
      updateTimeDependingAudio();
      
      //Updates die alleen moeten lopen als er een bepaalde conditie is in de game is
      if(globalSettings.difficulty == WRONG_ANSWER_TIME_X2) 
      {
        updateVirtualTime();
      }
    }

  }
}
//WARN : Lupart 0 word zo wel gebruikt voor audio als voor debuging kan problemen veroorzaken

//WARN : Let op of new data in dev_page.c goed word gereset
//WARN : check of de niew setPin en getPint werkt
//WARN : controller of tutchsensor op pin 1_4 werkt
//WARN : controleer of updateSpecialActies() werkt
//WARN : De function voor gesenureert is to gevoegt maar moet nog getest worden
//WARN : audio module is toegevoegd, moet nog goed getest worden en geintergreert/ audio aan toe gevoegt worden



//DONE: pin inishilation van de key en switch sensor toevoegen in switch_and_key_sensors_init() en de read functies invullen
//DONE: Taal van display templates wort nu aan gepast aan de hand van de audio instelling, hier voor een aparte instelling voor maken dat je hebt audio aan/ uit en taal NL/EN
//DONE: uitzoeken hoe lupart0 werkt.
//DONE: audio uitzoeken en triggers voor die aurio maken.

//DONE : In global setting bool maken voor gesensureert en uit audio halen. dit ook aanpassen in websiten en esp na aanlijding hier van de juiste template en audio evecten gebruiken.
//DONE: De esp een byte late stuuren als er setting verandert zijn, en allen dan de settings opnieuw laten ophalen, dit voorkomt dat er onnodig vaak settings opgehaald worden van de esp of de nxp late vragen of er niewe settings zijn
//IDEA: In send run data rety function maken
//IDEA: MS_PER_TICK_PANALTY variable maken tot betrekking tot globalSetting.difeculty
//IDEA: ver gezocht maar voor de website audio kunnen uploden, zeggen welke audio hij moet vervangen b.v nederlants dom has bin planted en die dan vervangen

//NOTE: Meschien de receive_room_settings_from_esp() ook allen uit voeren als de data verandert is (riziko hier van is dat je een keer de data verandert bit mis loopt en niet de actuele data hebt)