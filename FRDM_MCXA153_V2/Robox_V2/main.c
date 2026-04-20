#include "fsm.h"
#include "time.h"
#include "lpuart0.h"
#include "lpuart1.h"
#include "lpuart2.h"
int main(void)
{
  millis_init();
  
  lpuart0_init(9600);
  lpuart1_init(9600);
  lpuart2_init(115200);
  
  FSM_init();
  

  while (1)
  {
    FSM_update();
  }
  return 0;
}

