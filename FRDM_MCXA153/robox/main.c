#include "board.h"

#include "fsm.h"
#include "serial.h"
#include "game_logic.h"
#include "time_millis.h"
#include "keypad.h"
#include <stdio.h>


int main(void)
{
    serial_init(115200);
    FSM_config();
    keyPad_init();
    millis_init();
    //Haal data uit epromp

    while(1)
    {
      FSM_runStateMachine();
      updateTimeGamePanaltuMillis();
    }
}

