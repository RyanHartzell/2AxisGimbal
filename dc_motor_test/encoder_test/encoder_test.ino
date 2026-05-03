const byte encoder1pinA = 2;//A pin -> the interrupt pin 2
const byte encoder1pinB = 4;//B pin -> the digital pin 4

//Motor drive L239D
const int forwardPin = 8;
const int backwardPin = 12; 
//int delay = 2000;
const int pwmPin = 10;

int position = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;
int target = 0; //explore gloabal target to let it move

void readEncoder(){
  int b = digitalRead(encoder1pinB);
  if(b>0){
    position++;
  }
  else{
    position--;
  }
}

void driveMotor(int power, bool forward)
{
  if(power <= 0)
  {
    stopMotors();
  }
  else
  {
    //float sendValue = 255 * (float) dutyCycle/100.0;
  
    if(forward)
    {
      moveForward();
      analogWrite(pwmPin, power);
    }
    else 
    {
      moveBackward();
      analogWrite(pwmPin, power); //modulate forward pin to go backward
    }
  }
}

void stopMotors()
{
    //brake motors
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
    analogWrite(pwmPin, 0);
}

void moveForward()
{
    //spin forward
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
}

void moveBackward()
{
     //spin backward
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(encoder1pinA, INPUT);
  pinMode(encoder1pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder1pinA), readEncoder, RISING);

  //Setup motor control
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);

  //Configure to spin forward
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
  int speed = 0;
  analogWrite(pwmPin, speed);
}

int loop_count = 0;

void loop() {
  //target position -> 700 encoder ticks should give a one full rotation
  /**/
  //int target = 0; //made target global for moving target

  loop_count++;

  if (loop_count % 1000 == 0) {
    target = target + 200;
  }
  
  float diff = abs(target-position);
  readEncoder();
  // put your main code here, to run repeatedly:
  //Serial.print(position);
  
  //PID Constants
  float kp = 1;
  float kd = 0.025;
  float ki = 0;

  //time difference
  long currT = micros();

  float deltaT = ((float)(currT-prevT))/1.0e6; //microsecond precision
  prevT = currT;
  readEncoder();
  //error
  int e = target-position; //may need to switch sign on error term (target - position) if control isn't working

  //derivative
  float dedt = (e-eprev)/(deltaT); //finite difference approximation

  //integral
  eintegral = eintegral + e*deltaT; //finite difference approximation

  //control signal
  float u = kp*e + kd*dedt + ki*eintegral;

  //motor power
  float pwr = fabs(u); //floating point absolute value
  if(pwr>255){
    pwr = 255;
  }

  bool forward = true;
  if(u<0){
    forward = false;
  }

  //drive the motor
  driveMotor(pwr, forward);

  //store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(position);
  Serial.println();
}

