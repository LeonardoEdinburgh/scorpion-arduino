#define throttlePin 2
#define steeringPin 3
volatile unsigned long timer_startT, timer_startS;
volatile int last_interrupt_timeT, last_interrupt_timeS; //calcSignal is the interrupt handler
volatile int throttleVal;
volatile int steeringVal;

int t1, t2, t3, t4;
int s1, s2, s3, s4;

void setup()
{
  timer_startS = 0;
  timer_startT = 0;
  attachInterrupt(digitalPinToInterrupt(throttlePin), calcSignalT, CHANGE);
  attachInterrupt(digitalPinToInterrupt(steeringPin), calcSignalS, CHANGE);
  Serial.begin(115200);
}

void loop()
{
 throttleVal = smoothT(map(throttleVal, 1000,2000,-100, 100));
 steeringVal = smoothS(map(steeringVal, 1000,2000,-100, 100));

  Serial.print(throttleVal);
  Serial.print("      ");
  Serial.println(steeringVal);
  delay(20);
}







void calcSignalT()
{
  last_interrupt_timeT = micros();
  if (digitalRead(throttlePin) == HIGH)
  {
    timer_startT = micros();
  }
  else
  {
    if (timer_startT != 0)
    {
      throttleVal = ((volatile int)micros() - timer_startT);
      timer_startT = 0;
    }
  }
}
void calcSignalS()
{
  last_interrupt_timeS = micros();
  if (digitalRead(steeringPin) == HIGH)
  {
    timer_startS = micros();
  }
  else
  {
    if (timer_startS != 0)
    {
      steeringVal = ((volatile int)micros() - timer_startS);
      timer_startS = 0;
    }
  }
}

int smoothS(int newData)
{
  s4 = s3; 
  s3 = s2;
  s2 = s1;
  s1 = newData;
  return ((s1+s2+s3+s4) / 4);
}
int smoothT(int newData)
{
  t4 = t3; 
  t3 = t2;
  t2 = t1;
  t1 = newData;
  return ((t1+t2+t3+t4) / 4);
}

