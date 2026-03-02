#include "board.h"

#include "fsm.h"
#include "serial.h"
#include "game_logic.h"


int main(void)
{
    serial_init(115200);
    FSM_config();

    //Haal data uit epromp

    while(1)
    {
       FSM_runStateMachine();
       updateTimeGamePanaltuMillis();
    }
}
