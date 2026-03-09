#define TOUCH_SENSOR_PIN 2
#define R_PIN 4
#define G_PIN 3
#define B_PIN 5
#define TOUCH_GLITCH_FILTER_MS  500
#define TOUCH_HOLD_TIME_MS  2000
void setup() 
{
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
  Serial.begin(9600);

  digitalWrite(R_PIN, HIGH);
  digitalWrite(G_PIN, HIGH);
  digitalWrite(B_PIN, HIGH);
}
void loop()
{
  isTouchLongPressed() ;
}
typedef enum 
{
  OFF,
  WHITE,
  RED,
  GREEN
}collors_t;

void setCollor(collors_t collor);

bool isTouchLongPressed() 
{
  static bool isPressing = false;
  static uint32_t pressStart = 0;
  static uint32_t lastValidPress = 0;

  bool state = digitalRead(TOUCH_SENSOR_PIN);
  uint32_t now = millis();

  if(state)
  {
    if(!isPressing)
    {
      pressStart = now;
      isPressing = true;
      setCollor(RED);
    }
    lastValidPress = now;
  }
  else if(isPressing)
  {
    if(now - lastValidPress > TOUCH_GLITCH_FILTER_MS)
    {
      isPressing = false;
      setCollor(WHITE);
    }
  }
  if(!isPressing) return false;

  if(now - pressStart >= TOUCH_HOLD_TIME_MS)
  {
    setCollor(GREEN);
    return true;
  }
}

void setCollor(collors_t collor)
{
  bool r = false;
  bool g = false;
  bool b = false;
  switch (collor) 
  {
    case OFF:

    break;
    case WHITE:
      g = true;
      r = true;
      b = true;
    break;
    case RED:
      r = true;
    break;
    case GREEN:
      g = true;
    break;
    default: 

    break;
  }
  digitalWrite(R_PIN, r);
  digitalWrite(G_PIN, g);
  digitalWrite(B_PIN, b);
}
