#define throttlePin 2
#define steeringPin 3
volatile unsigned long timer_startT, timer_startS;
volatile int last_interrupt_timeT, last_interrupt_timeS; //calcSignal is the interrupt handler
volatile int throttleVal;
volatile int steeringVal;
int t1, t2, t3, t4, t5, t6, t7, t8;
int s1, s2, s3, s4, s5, s6, s7, s8;
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

 if (-5 <= throttleVal && throttleVal <= 5)
 {
  throttleVal = 0;
 }
  if (-5 <= steeringVal && steeringVal <= 5)
 {
  steeringVal = 0;
 }
 for (int i = 0; i < 100; i++)
 {
  Serial.print(i);
  Serial.print("_");
  Serial.println(i);
  delay(20);
 }
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
  s8 = s7;
  s7 = s6;
  s6 = s5;
  s5 = s4;
  s4 = s3; 
  s3 = s2;
  s2 = s1;
  s1 = newData;
  return ((s1+s2+s3+s4+s5+s6+s7+s8) / 8);
}
int smoothT(int newData)
{
  t8 = t7;
  t7 = t6;
  t6 = t5;
  t5 = t4;
  t4 = t3; 
  t3 = t2;
  t2 = t1;
  t1 = newData;
  return ((t1+t2+t3+t4+t5+t6+t7+t8) / 8);
}

