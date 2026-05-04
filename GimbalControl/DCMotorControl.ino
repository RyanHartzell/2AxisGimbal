/*
  FIT0186 DC 12V Motor

  https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/2523/FIT0186_Web.pdf
*/

/*
  Motor pins
*/
const byte encoder1pinA = 2; // A pin -> the interrupt pin 2
const byte encoder1pinB = 4; // B pin -> the digital pin 4
//// PCB ////
bool pcb = true;
const int forwardPin = 10;
const int backwardPin = 11;
const int pwmPin = -1; // to prevent compile errors, not actually used for pcb
/////////////
//// Motor drive L239D (breadboard test) ////
// bool pcb = false;
// const int forwardPin = 8;
// const int backwardPin = 12; 
// const int pwmPin = 10;
/////////////////////////////////////////////

/*
  PWM command limits
*/
int minPWM = 300; // 30;
int maxPWM = 500; // 120;

// if the motor should be able to move
bool drive = true;

// motor direction
boolean Direction = true;
int encoder1PinALast = 0;

/*
  PID controller variables
*/
float kp = 0.7; //1;
float kd = 0.05; //0.025;
float ki = 0; //0;

long prevT = 0;
float eprev = 0;
float eintegral = 0;

// error tolerance for the position target
int errorThreshold = 10;
// current position in encoder counts
int encPosition = 0; // updated by readEncoders during interrupts
// target position in encoder counts
int encTarget = 0;
// number of encoder counts in one full motor rotation
int encCountsPerRev = 1400;


/*
  Set up motor pins and encoder interrupt
*/
void setupDCMotor() {

  // set up encoder pins
  // pinMode(encoder1pinA, INPUT); // testing commented out
  pinMode(encoder1pinB, INPUT); // testing commented out
  attachInterrupt(digitalPinToInterrupt(encoder1pinA), readEncoder, CHANGE); //interrupt port 0 connected to digital pin 2

  if (pcb) { // code for the PCB
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);
  }
  else { // code for motor drive L239D breadboard test
    // Setup motor control pins
    pinMode(forwardPin, OUTPUT);
    pinMode(backwardPin, OUTPUT);

    // Configure to spin forward, set speed to 0
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
    analogWrite(pwmPin, 0);
  }

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
  if (currDegreePos < 180) {
    DCMoveTo(0);
  }
  // if the motor is at less than 180 degrees, command to move to 0 to move backwards
  else {
    DCMoveTo(-360);
  }

}

int loop_count = 0; // DEBUG

/*
  Move the DC motor to the target position using PID control
  Must be called in the program's main loop() function
*/
void DCMotorControlLoop() {

  if (!drive) { return; } // only try and drive the motor if the motor wasn't stopped by a command

  // calculate time difference
  long currT = micros();
  float deltaT = ((float)(currT - prevT)) / 1.0e6; // microsecond precision
  prevT = currT;

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
  if (loop_count % 90000 == 0) {
    Serial.print("In DC loop: encTarget = ");
    Serial.print(encTarget);
    Serial.print(" encPosition = ");
    Serial.print(encPosition);
    Serial.print(" error = ");
    Serial.print(e);
    Serial.print("   power command: ");
    Serial.println(pwr);
  }

  bool forward = true;
  if (u < 0) {
    forward = false;
  }

  // TESTING //
  // if the error is small enough, don't try and correct it
  if (abs(e) < errorThreshold) { pwr = 0; } //encTarget = encPosition; }

  // drive the motor
  driveMotor(pwr, forward);

  // store previous error
  eprev = e;

}

void readEncoder() {

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
  
  if(pcb) { // code for pcb

    if(power <= 0)
    {
      stopMotor();
    }
    else
    {    
      if (forward) {
        moveForward(power);
      }
      else { // backwards
        moveBackward(power);
      }
    }
  }
  else { // code for motor drive L239D breadboard test

    if (power <= 0) {
      stopMotor();
    }
    else {
    
      if (forward) {

        _moveForward(); // modulate direction pins to go forward
        analogWrite(pwmPin, power);

      }
      else {

        _moveBackward(); // modulate direction pins to go backward
        analogWrite(pwmPin, power);

      }
    }
  }
}

void stopMotor() {

  drive = false;
  
  if (pcb) { // code for pcb
    // ensure power is 0
    moveForward(0);
    moveBackward(0);
    // stop motor
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, LOW);
  }
  else { // code for motor drive L239D breadboard test
    // stop motor
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    analogWrite(pwmPin, 0);
  }
}

void moveForward(int power) {

  digitalWrite(forwardPin, LOW);
  analogWrite(backwardPin, power); // modulate reverse pin to go forward
}

void moveBackward(int power) {

  digitalWrite(backwardPin, LOW);
  analogWrite(forwardPin, power); // modulate forward pin to go backward
}


void _moveForward() { // only for motor drive L239D breadboard test
  
   // spin forward
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
}

void _moveBackward() { // only used for motor drive L239D breadboard test

  // spin backward
  digitalWrite(forwardPin, LOW);
  digitalWrite(backwardPin, HIGH);
}

