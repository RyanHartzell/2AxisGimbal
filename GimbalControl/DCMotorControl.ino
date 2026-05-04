#include <Encoder.h>

/*
  Motor pins
*/
const byte encoder1pinA = 2; // A pin -> the interrupt pin 2
const byte encoder1pinB = 4; // B pin -> the digital pin 4

long oldPosition  = -999;
Encoder myEnc(encoder1pinA, encoder1pinB);

// Motor drive L239D
const int forwardPin = 8;
const int backwardPin = 12; 
const int pwmPin = 10;

int minPWM = 35;
int maxPWM = 130;

bool drive = true;

/*
  PID controller variables
*/
float kp = 0.8; //1;
float kd = 0.05; //0.025;
float ki = 0; //0;

int errorThreshold = 10;

int encPosition = 0; // updated by readEncoders during interrupts
long prevT = 0;
float eprev = 0;
float eintegral = 0;

int encTarget = 0; // in encoder ticks, 700 encoder ticks should give a one full rotation

int encCountsPerRev = 1400; // number of encoder counts in one full motor rotation

boolean Direction = true;
int encoder1PinALast = 0;

void setupDCMotor() {

  // set up encoder pins
  // pinMode(encoder1pinA, INPUT); // testing commented out
  pinMode(encoder1pinB, INPUT); // testing commented out
  attachInterrupt(digitalPinToInterrupt(encoder1pinA), readEncoder, CHANGE); //interrupt port 0 connected to digital pin 2

  // Setup motor control pins
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);

  // Configure to spin forward, set speed to 0
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
  analogWrite(pwmPin, 0);

}

/*
  Set the position to move the DC motor to
  DCMotorControlLoop() must be called in the main loop() function to work

  @param degree: [int] degree to rotate the motor to: [0, 360], +/- for forward/backward (can be given input >360 to do more than one rotation)
*/
void DCMoveTo(int degree) {

  // allow the motor to drive
  drive = true;

  // Serial.print("Start of DCMoveTo: ----- degree = ");
  // Serial.println(degree);
  
  //encTarget = encPosition;

  // Serial.print("In DCMoveTo: encPosition = ");
  // Serial.println(encPosition);

  // TODO: need?
  // if (encPosition > 99999) {
  //   // reset encoder counts and encTarget to avoid them getting too large? modded in function so it should keep global position the same
  //   resetEncoder();
  // }
  
  // convert where motor is currently to degrees
  int wrappedEncPos = encPosition % (encCountsPerRev + 1); // only care about position on the circle, not number of rotations completed
  int currDegreePos = wrappedEncPos * (360.0 / encCountsPerRev);

  // Serial.print("In DCMoveTo: wrappedEncPos = ");
  // Serial.println(wrappedEncPos);
  // Serial.print("In DCMoveTo: currDegreePos = ");
  // Serial.println(currDegreePos);



  // calculate degree distance between current position and target position
  int degreesToMove = degree - currDegreePos;

  // TODO: make motor take the shortest path instead of enforcing positive degree = forward and negative degree = backwards?
  // TOOD: motor struggles to be accurate when the command is the same position over and over again, probably related to above ^^

  // Serial.print("In DCMoveTo: degreesToMove = ");
  // Serial.println(degreesToMove);
  

  // convert degree target to encoder counts
  int newEncTarget = degreesToMove * (encCountsPerRev / 360.0);

  // Serial.print("In DCMoveTo: newEncTarget = ");
  // Serial.println(newEncTarget);

  // add new target to encTarget (encoder counts do not reset each revolution, so target cannot simply be set to a value [0,700])
    // if degree input was negative, encTarget will decrease and motor will go backwards
  encTarget = encTarget + newEncTarget;

  // DCMotorControlLoop will handle moving the motor to the target with PID control

  Serial.print("DCMoveTo: degrees = ");
  Serial.print(degree);
  Serial.print("  encTarget = ");
  Serial.println(encTarget);

}

/*
  Return the DC motor to the 0/360 position
  Move either forwards or backwards depending on current rotation, whichever direction makes more sense
*/
void DCResetMotor() {

  // allow the motor to drive
  drive = true;

  // convert where motor is currently to degrees
  int wrappedEncPos = encPosition % (encCountsPerRev + 1); // only care about position on the circle, not number of rotations completed
  int currDegreePos = wrappedEncPos * (360.0 / encCountsPerRev);

  // if the motor is at or past 180 degrees, command to move to 360 to complete the forward rotation
  if (currDegreePos >= 180) {
    DCMoveTo(360);
  }
  // if the motor is at less than 180 degrees, command to move to 0 to move backwards
  else {
    DCMoveTo(0);
  }

}

int loop_count = 0; // DEBUG
/*
  Move the DC motor to the target position using PID control
  Must be called in the program's main loop() function
*/
void DCMotorControlLoop() {

  if (!drive) { return; } // only try and drive the motor if the motor wasn't stopped by a command
  // readEncoder();

  // calculate time difference
  long currT = micros();
  float deltaT = ((float)(currT - prevT)) / 1.0e6; // microsecond precision
  prevT = currT;

  // readEncoder();

  // calculate error
  int e = encTarget - encPosition; // may need to switch sign on error term (target - position) if control isn't working

  // error derivative
  float dedt = (e - eprev) / deltaT; // finite difference approximation

  // error integral
  eintegral = eintegral + e * deltaT; // finite difference approximation

  // get control signal
  float u = kp * e + kd * dedt + ki * eintegral;

  // convert to motor power
  float pwr = fabs(u); // floating point absolute value
  if (pwr > maxPWM) { // limit to max power
    pwr = maxPWM;
  }
  else if (pwr < minPWM) { // limit to min power that actually moves the motor
    pwr = minPWM;
  }

  // DEBUGGING: print statements make the loop not get called as often, need to take out for final tuning and actual performance
  // if (loop_count % 90000 == 0) {
  //   Serial.print("In DC loop: encTarget = ");
  //   Serial.print(encTarget);
  //   Serial.print(" encPosition = ");
  //   Serial.print(encPosition);
  //   Serial.print(" error = ");
  //   Serial.print(e);
  //   Serial.print("   power command: ");
  //   Serial.println(pwr);
  // }

  bool forward = true;
  if (u < 0) {
    forward = false;
  }

  // TESTING //
  // if the error is small enough, don't try and correct it
  if (abs(e) < errorThreshold) { pwr = 0; }//encTarget = encPosition; }

  // drive the motor
  driveMotor(pwr, forward);

  // store previous error
  eprev = e;

}

void readEncoder() {

  //// from pid controller code ////
  // int b = digitalRead(encoder1pinB);
  
  // if (b>0) { encPosition++; }
  // else { encPosition--; }

  //// TEST using encoder.h library ////
  //encPosition = myEnc.read();

  int Lstate = digitalRead(encoder1pinA);
  if((encoder1PinALast == LOW) && Lstate==HIGH)
  {
    int val = digitalRead(encoder1pinB);
    if(val == LOW && Direction)
    {
      Direction = false; //Reverse
    }
    else if(val == HIGH && !Direction)
    {
      Direction = true;  //Forward
    }
  }
  encoder1PinALast = Lstate;

  if(!Direction)  encPosition--;
  else  encPosition++;

}

void resetEncoder() {
  encPosition = 0; //encPosition % (encCountsPerRev + 1);
  encTarget = encPosition;
}

int getEncoderCount() {
  return encPosition;
}

void driveMotor(int power, bool forward) {
  
  if (power <= 0) {
    stopMotor();
  }
  else {
  
    if (forward) {

      moveForward(); // modulate direction pins to go forward
      analogWrite(pwmPin, power);

    }
    else {

      moveBackward(); // modulate direction pins to go backward
      analogWrite(pwmPin, power);

    }
  }
}

void stopMotor() {

  // brake motors
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, HIGH);
  analogWrite(pwmPin, 0);

  drive = false;

}

void moveForward() {

  // spin forward
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);

}

void moveBackward() {

  // spin backward
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);

}

