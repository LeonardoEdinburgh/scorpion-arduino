
// The Sparkfun Monster Moto Sheild PWM inputs are rated at 20kHz and the minimum practical pulse inout is about 4 us.
// sheild pin assignments
#define PinInA1 7
#define PinInA2 4
#define PinInB1 8
#define PinInB2 9
#define PinPWM1 5
#define PinPWM2 6
#define PinCS1 A2
#define PinCS2 A3
#define PinEN1 A0
#define PinEN2 A1
#define LED    13


// shit for serial
const byte numChars = 64;
char receivedChars[numChars];   // an array to store the received data
char command[numChars];
boolean newData = false;
int receivedSpeed = 0 ;


// Timer2 compare A interrupt service routine
ISR(TIMER2_COMPA_vect)
{
  // Turn on PWM pins 5 and 6
  //      76543210 Arduino digital pin number
  PORTD |= B01100000;
}

ISR(TIMER2_COMPB_vect)      // Timer2 compare B interrupt service routine
{
  // Turn off PWN pins 5 and 6
  //      76543210 Arduino digital pin number
  PORTD &= B10011111;
}

void setPWMDuty(int duty)
// PWM duty percent 8% to 92%
{
  duty = duty - 1;      // 0 to 99 equals 100
  if (duty < 7) duty = 7; // Minimum pulse width is 4us (see VNH2SP30-E datasheet Table 9)
  if (duty > 91) duty = 91; // Minimum pulse width is 4us (see VNH2SP30-E datasheet Table 9)
  OCR2B = duty;
}

void setup()
{
  Serial.begin(9600);
  // Initialise H-Bridge (Monster Moto)
  pinMode(PinInA1, OUTPUT);
  pinMode(PinInA2, OUTPUT);
  pinMode(PinInB1, OUTPUT);
  pinMode(PinInB2, OUTPUT);
  pinMode(PinPWM1, OUTPUT);
  pinMode(PinPWM2, OUTPUT);
  pinMode(PinCS1, INPUT);
  pinMode(PinCS2, INPUT);
  pinMode(PinEN1, OUTPUT);
  pinMode(PinEN2, OUTPUT);
  digitalWrite(PinEN1, HIGH);
  digitalWrite(PinEN2, HIGH);
  digitalWrite(PinInA1, LOW);
  digitalWrite(PinInA2, LOW);
  digitalWrite(PinInB1, LOW);
  digitalWrite(PinInB2, LOW);
  digitalWrite(PinPWM1, 25);
  digitalWrite(PinPWM2, 25);

  // Initialize timer10
  noInterrupts();           // Disable all interrupts
  TCCR2A = B00000010;       // Disconnect Arduino D3 and D11 pins and set CTC mode (see AVR datasheet S11.11.1)
  TCCR2B = B00000010;       // Set clock prescaler to 8, now clock is 2MHz (see AVR datasheet S11.11.2)
  TCNT2  = B00000000;       // Reset timer (why not?) (see AVR datasheet S18.11.3)
  OCR2A  = 99;              // Set compare match register A for 20kHz PWM frequency (see AVR datasheet S18.11.4)
  OCR2B  = 7;               // OCR2B (see AVR datasheet S18.11.5) to duty, 1% per step, minimum pulse width is 4 us.
  TIMSK2 = B00000110;       // Enable timer compare interrupt on OCR2A ans 0CR2B (see AVR datasheet S18.11.6)
  interrupts();             // Enable all interrupts

  // PWM duty percent, valid values are between 8% to 92% (limits are imposed)
  setPWMDuty(20);          // 2% is a good alround value for most motors
}

#define CW 1
#define CCW -1
#define Stopped 0
int motorState = 0;
int motorDirection = Stopped; // Stopped
int motorDelay = 5;         // 60 RPM
int PwmDutyVal = 0;
int recievedInt = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
 //recvWithStartEndMarkers();
 showNewData();
 
  if (receivedSpeed > 3500) {
    PwmDutyVal = 35;
  }
  else if (receivedSpeed > 2500)  {
    PwmDutyVal = 40;
  }
  else if (receivedSpeed > 2000)  {
    PwmDutyVal = 45;
  }
  else if (receivedSpeed > 1700)  {
    PwmDutyVal = 50;
  }
  else if (receivedSpeed > 1400)  {
    PwmDutyVal = 55;
  }
  else if (receivedSpeed > 1200)  {
    PwmDutyVal = 65;
  }
  else if (receivedSpeed > 1000)  {
    PwmDutyVal = 75;
  }
  else if (receivedSpeed >= 850)  {
    PwmDutyVal = 92;
  }
  //  Serial.println(receivedSpeed);
  //   Serial.println(PwmDutyVal);

  motorDelay = receivedSpeed;    // 60 RPM for a 200 steps motora  700 = 100% ???= 0%
  // Set nominal current
  setPWMDuty(PwmDutyVal);

  if (motorDirection == Stopped)
  {
    digitalWrite(PinInA1, LOW); digitalWrite(PinInB1, LOW);
    digitalWrite(PinInA2, LOW); digitalWrite(PinInB2, LOW);
  }
  else
  {
    // Full step
    if (motorState == 0) {
      digitalWrite(PinInA1, HIGH); digitalWrite(PinInB1, LOW);
      digitalWrite(PinInA2, LOW); digitalWrite(PinInB2, HIGH);
      delayMicroseconds(motorDelay);
      motorState += motorDirection;
    } else if (motorState == 1) {
      digitalWrite(PinInA1, LOW); digitalWrite(PinInB1, HIGH);
      digitalWrite(PinInA2, LOW); digitalWrite(PinInB2, HIGH);
      delayMicroseconds(motorDelay);
      motorState += motorDirection;
    } else if (motorState == 2) {
      digitalWrite(PinInA1, LOW); digitalWrite(PinInB1, HIGH);
      digitalWrite(PinInA2, HIGH); digitalWrite(PinInB2, LOW);
      delayMicroseconds(motorDelay);
      motorState += motorDirection;
    } else if (motorState == 3) {
      digitalWrite(PinInA1, HIGH); digitalWrite(PinInB1, LOW);
      digitalWrite(PinInA2, HIGH); digitalWrite(PinInB2, LOW);
      delayMicroseconds(motorDelay);
      motorState += motorDirection;
    }
    motorState = (motorState + 4) % 4;

  }
}

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  
 // if (newData == true) {
  //  Serial.println(Serial.available() );
    while (Serial.available() > 0) {
      recievedInt = Serial.read();
//    }
    Serial.println(recievedInt);
    String str = String(receivedChars);
    receivedSpeed = str.toInt();

    // Safety check
    if (receivedSpeed < 101 && receivedSpeed > -101)
    {
      if (receivedSpeed > 0)
      {
        receivedSpeed = map(receivedSpeed, 1, 100, 10000, 850);
        motorDirection = CW;
      }
      else if (receivedSpeed < 0)
      {
        receivedSpeed = map(receivedSpeed, -1, -100, 10000, 850);
        motorDirection = CCW;
      }
      else
      {
        receivedSpeed = 0;
        motorDirection = Stopped;
      }
    }
    else
    {
      receivedSpeed = 0;
      motorDirection = Stopped;
    }
  
  memset(receivedChars, 0, sizeof(receivedChars));
  newData = false;
}
}



