#include "board.h"

#include "fsm.h"
#include "serial.h"

int main(void)
{
    serial_init(115200);
    FSM_config();

    while(1)
    {
       FSM_runStateMachine();
    }
}
