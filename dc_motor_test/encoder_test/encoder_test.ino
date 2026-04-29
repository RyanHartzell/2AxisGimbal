const byte encoder0pinA = 2;//A pin -> the interrupt pin 2
const byte encoder0pinB = 4;//B pin -> the digital pin 4

//Motor drive L239D
const int forwardPin = 8;
const int backwardPin = 12; 
int delay = 2000;
const int pwmPin = 10;

int position = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void readEncoder(){
  int b = digitalRead(encoder0pinB);
  if(b>0){
    pos++;
  }
  else{
    pos--;
  }
}

void driveMotor(int power, bool forward)
{
  if(dutyCycle <= 0)
  {
    stopMotors();
  }
  else
  {
    //float sendValue = 255 * (float) dutyCycle/100.0;
    float sendValue = power;
  
    if(forward)
    {
      moveForward();
      if(dutyCycle >= 100) 
      {
        analogWrite(pwmPin, 255);
      }
      else
      {
        analogWrite(pwmPin, (int) sendValue); //modulate reverse pin to go forward
      }
    
    }
    else 
    {
      moveBackward();
      if(dutyCycle >= 100 
      {
        analogWrite(pwmPin, 255);
      }
      else
      {
        analogWrite(pwmPin, (int) sendValue); //modulate forward pin to go backward
      }
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
  pinMode(encoder0pinA, INPUT);
  pinMode(encoder0pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder0pinA), readEncoder, RISING);

  //Setup motor control
  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);

  //Configure to spin forward
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
  speed = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(position);

  //target position -> 700 encoder ticks should give a one full rotation
  int target = 700;

  //PID Constants
  float kp = 1;
  float kd = 0.025;
  float ki = 0;

  //time difference
  long currT = micros();

  float deltaT = ((float)(currT-prevT))/1.0e6; //microsecond precision
  prevT = currT;

  //error
  int e = position-target; //may need to switch sign on error term (target - position) if control isn't working

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
  Serial.print(pos);
  Serial.println();
}

