#define TICK_DURATION 1000
#define ROWS 8
#define COLOMS 16

typedef enum
{
  NORHT,
  EAST,
  SOUTH,
  WEST
}derections_t;
typedef struckt
{
  uint8_t row;
  uint8_t colom;
}snakePoint_t;

bool grit[ROWS][COLOMS] = 
{
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
snakePoint_t snake[ROWS*COLOMS];
snakePoint_t appel;
uint8_t snakeLenkt = 1;

void setup() 
{
  	

}

void loop() 
{
  static uint32_t lastTickUpdate;
  uint32_t now = millis();
  if(now - lastTickUpdate < TICK_DURATION) return;

  snakePoint_t head = &snake[0];
  if(head.row > ROWS || head.colom > COLOMS) return; //  // je bent dood


  grit[snake[snakeLenkt].row][snake[snakeLenkt].colom] = false;
  for(int i = snakeLenkt; i > 1; i--)
  {
    snake[i-1] = snake[i];
    
  }
  aplyDerection(&head);
  grit[head.row][head.colom] = true;


  for(int i = 1; i < snakeLenkt; i++)
  {
    if(head.row == snake[i].row && head.colom == snake[i].colom)
    {
      return; // je bent dood
    }
  }


  if(appel.row == head.row && appel.comom == head.colom) 
  {
    snakeLenkt++;
    appel.row = 0;//  ranom
    appel.colom = 0;// random
  }


  fisueel();
}

void fisueel()
{
  for (uint8_t y = 0; y < ROWS; y++) 
  {
    for (uint8_t x = 0; x < COLOMS; x++) 
    {
      bool isSnake = false;
      for(uint8_t i = 0; i < snakeLenkt; i++)
      {
        if(snake[i].row == y && snake[i].colom == x ) isSnake = true;
      }
      if(appel.row == y && appel.colom == x) isSnake = true;
      
      Serial.print("%s", isSnake? "[#]": "[ ]");
    }
    Serial.print("\n");
  }
} 

void aplyDerection(snakePoint_t &point)
{
  derections_t der = NORHT;
  switch (der) 
  {
    case NORHT:
    point.ROWS ++;
    break;
    case EAST:
    point.ROWS --;
    break;
    case SOUTH:
    point.colom ++;
    break;
    case WEST:
    point.colom --
    break;
  }
} 
