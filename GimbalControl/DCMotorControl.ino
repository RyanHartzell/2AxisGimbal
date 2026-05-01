/*
  Motor pins
*/
const byte encoder1pinA = 2; // A pin -> the interrupt pin 2
const byte encoder1pinB = 4; // B pin -> the digital pin 4

// Motor drive L239D
const int forwardPin = 8;
const int backwardPin = 12; 
const int pwmPin = 10;

/*
  PID controller variables
*/
float kp = 1;
float kd = 0.025;
float ki = 0;

int encPosition = 0; // updated in DCMotorControlLoop by readEncoders
long prevT = 0;
float eprev = 0;
float eintegral = 0;

int encTarget = 0; // in encoder ticks, 700 encoder ticks should give a one full rotation

int encCountsPerRev = 700; // number of encoder counts in one full motor rotation


void setupDCMotor() {

  // set up encoder pins
  pinMode(encoder1pinA, INPUT);
  pinMode(encoder1pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder1pinA), readEncoder, RISING);

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

  // TODO: want to ensure motor has reached the last target first?
      // would probably have to add a loop with a delay to check if target is reached yet and only update the target after it is reached (or after a certain number of loops)
      // could also be fine this way as long as the motor is given enough time to get to the target or at least close enough for the camera reading
  // reset encTarget to current position in case motor has not reached previous target
  readEncoder();
  encTarget = encPosition;
  
  // convert where motor is currently to degrees
  int wrappedEncPos = encPosition % (encCountsPerRev + 1); // only care about position on the circle, not number of rotations completed
  int currDegreePos = wrappedEncPosition * (360 / encCountsPerRev);

  // calculate degree distance between current position and target position
  int degreesToMove = degree - currDegreePos;

  // convert degree target to encoder counts
  int newEncTarget = degreesToMove * (encCountsPerRev / 360);

  // add new target to encTarget (encoder counts do not reset each revolution, so target cannot simply be set to a value [0,700])
    // if degree input was negative, encTarget will decrease and motor will go backwards
  encTarget = encTarget + newEncTarget;

  // DCMotorControlLoop will handle moving the motor to the target with PID control

}

/*
  Return the DC motor to the 0/360 position
  Move either forwards or backwards depending on current rotation, whichever direction makes more sense
*/
void DCResetMotor() {

  // convert where motor is currently to degrees
  int wrappedEncPos = encPosition % (encCountsPerRev + 1); // only care about position on the circle, not number of rotations completed
  int currDegreePos = wrappedEncPosition * (360 / encCountsPerRev);

  // if the motor is at or past 180 degrees, command to move to 360 to complete the forward rotation
  if (currDegreePos >= 180) {
    DCMoveTo(360);
  }
  // if the motor is at less than 180 degrees, command to move to 0 to move backwards
  else {
    DCMoveTo(0);
  }

}

/*
  Move the DC motor to the target position using PID control
  Must be called in the program's main loop() function
*/
void DCMotorControlLoop() {

  readEncoder();

  // calculate time difference
  long currT = micros();
  float deltaT = ((float)(currT - prevT)) / 1.0e6; // microsecond precision
  prevT = currT;

  readEncoder();

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
  if (pwr > 255) { // limit to max power
    pwr = 255;
  }

  bool forward = true;
  if (u < 0) {
    forward = false;
  }

  // drive the motor
  driveMotor(pwr, forward);

  // store previous error
  eprev = e;

}

void readEncoder() {

  int b = digitalRead(encoder1pinB);
  
  if (b>0) { encPosition++; }
  else { encPosition--; }

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

