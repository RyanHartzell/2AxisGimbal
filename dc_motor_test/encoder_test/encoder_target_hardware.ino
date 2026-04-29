//Encoder pins
const byte encoder0pinA = 2;//A pin -> the interrupt pin 2
const byte encoder0pinB = 4;//B pin -> the digital pin 4
/* Attempt at code to drive a dc motor using DRV8231A H-Bridge*/
const int forwardPin = 9; //HBridge IN1
const int backwardPin = 11; //HBridge IN2
const int delayTime = 2000;
bool demoed = false;

int position = 0;
long prevT = 0;
float eprev = 0;
float eintegral = 0;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(encoder0pinA, INPUT);
  pinMode(encoder0pinB, INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder0pinA), readEncoder, RISING);

  //Configure motor pins
  pinMode(forwardPin,OUTPUT);
  pinMode(backwardPin,OUTPUT);
}

void readEncoder(){
  int b = digitalRead(encoder0pinB);
  if(b>0){
    pos++;
  }
  else{
    pos--;
  }
}

void driveMotor(int dutyCycle, bool forward)
{
  if(dutyCycle <= 0)
  {
    stopMotors();
  }
  else
  {
    int inverseDutyCycle = 100 - dutyCycle; 
    float sendValue = 255 * (float) inverseDutyCycle/100.0;
  
    if(forward)
    {
      if(dutyCycle >= 100) 
      {
        fullForward();
      }
      else
      {
        digitalWrite(forwardPin, HIGH);
        analogWrite(backwardPin, (int) sendValue); //modulate reverse pin to go forward
      }
    
    }
    else 
    {
      if(dutyCycle >= 100 
      {
        fullReverse();
      }
      else
      {
        digitalWrite(backwardPin, HIGH);
        analogWrite(forwardPin, (int) sendValue); //modulate forward pin to go backward
      }
    }
  }
}

void stopMotors()
{
    //brake motors
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, HIGH);
}

void fullForward()
{
    //spin forward
    digitalWrite(forwardPin, HIGH);
    digitalWrite(backwardPin, LOW);
}

void fullReverse()
{
     //spin backward
    digitalWrite(forwardPin, LOW);
    digitalWrite(backwardPin, HIGH);
}

void loop() 
{
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
  float dutyCycleF = 0;
  if(pwr>255){
    pwr = 255;
    dutyCycle = (pwr/255.0)*100.0;
  }
  int dutyCycleI = (int) dutyCycle; //converting from pwm to duty cycle back to pwm. crime against humanity

  bool forward = true;
  if(u<0){
    forward = false;
  }

  //drive the motor
  driveMotor(dutyCycleI, forward);

  //store previous error
  eprev = e;

  Serial.print(target);
  Serial.print(" ");
  Serial.print(pos);
  Serial.println();
}

