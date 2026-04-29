/* Attempt at code to drive a dc motor using DRV8231A H-Bridge*/
const int forwardPin = 9; //HBridge IN1
const int backwardPin = 11; //HBridge IN2
const int delayTime = 2000;
bool demoed = false;

void setup() 
{
  pinMode(forwardPin,OUTPUT);
  pinMode(backwardPin,OUTPUT);
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
    if(!demoed)
    {
      //spin forward
      fullForward();
      delay(delayTime);

      //spin backward
      fullReverse();
      delay(delayTime);

      //brake motors
      stopMotors();
      delay(delayTime);

      //PWM - Analog duty cycle writes to IN1 and IN2
      //Calculate PWM max and create an equation to effectively send duty cycles
      driveMotors(50, true);
      delay(delayTime);
      driveMotors(25, true);
      delay(delayTime);
      driveMotors(50, false);
      delay(delayTime);
      driveMotors(25, false);
      delay(delayTime);

      //Drive Forward at 50% duty cycle. Set reverse pin to 128 and keep forward pin at 1.
      //Vice-Versa to spin backwards
    }
    demoed = true;
}