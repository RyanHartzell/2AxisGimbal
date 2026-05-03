
//The sample code for driving one way motor encoder
const byte encoder0pinA = 2;//A pin -> the interrupt pin 2
const byte encoder0pinB = 4;//B pin -> the digital pin 4
const byte encoder0pinBpwm = 4;
byte encoder0PinALast;
int duration;//the number of the pulses
boolean Direction;//the rotation direction


const int forwardPin = 8;
const int backwardPin = 12; 
const int pwmPin = 10;

void setup()
{
  Serial.begin(9600);//Initialize the serial port
  EncoderInit();//Initialize the module

  pinMode(forwardPin, OUTPUT);
  pinMode(backwardPin, OUTPUT);

  // Configure to spin forward, set speed
  digitalWrite(forwardPin, HIGH);
  digitalWrite(backwardPin, LOW);
  analogWrite(pwmPin, 60);

  /**/
  //// TEST
  //pinMode(encoder0pinA, INPUT);
  //pinMode(encoder0pinB, INPUT);
  ////
  


}

void loop()
{
  /*
  //// TEST
  digitalWrite(encoder0pinA, LOW);
  delay(100);
  digitalWrite(encoder0pinB, HIGH);
  digitalWrite(encoder0pinA, HIGH);
  delay(1000);

  digitalWrite(encoder0pinA, LOW);
  delay(100);
  digitalWrite(encoder0pinB, LOW);
  digitalWrite(encoder0pinA, HIGH);
  delay(1000);
  ////
  */
  
  
  Serial.print("Pulse:");
  Serial.println(duration);
  // duration = 0;
  delay(100);
  

  // digitalWrite(encoder0pinA, HIGH);
  // delay(4000);
  // digitalWrite(encoder0pinA, LOW);
  // delay(4000);
}

void EncoderInit()
{
  Direction = true;//default -> Forward
  pinMode(encoder0pinB,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), wheelSpeed, CHANGE); //interrupt port 0 connected to digital pin 2
}

void wheelSpeed()
{
  int Lstate = digitalRead(encoder0pinA);
  if((encoder0PinALast == LOW) && Lstate==HIGH)
  {
    int val = digitalRead(encoder0pinB);
    if(val == LOW && Direction)
    {
      Direction = false; //Reverse
    }
    else if(val == HIGH && !Direction)
    {
      Direction = true;  //Forward
    }
  }
  encoder0PinALast = Lstate;

  if(!Direction)  duration  ;
  else  duration--;
}

// const byte encoder0pinA = 2;//A pin -> the interrupt pin 0
// const byte encoder0pinB = 4;//B pin -> the digital pin 4
// byte encoder0PinALast;
// int duration;//the number of the pulses
// boolean Direction;//the rotation direction

// // Motor drive L239D
// const int forwardPin = 8;
// const int backwardPin = 12; 
// const int pwmPin = 10;


// void setup()
// {
//  Serial.begin(9600);//Initialize the serial port
//  EncoderInit();//Initialize the module

//   pinMode(forwardPin, OUTPUT);
//   pinMode(backwardPin, OUTPUT);

//   // Configure to spin forward, set speed
//   digitalWrite(forwardPin, HIGH);
//   digitalWrite(backwardPin, LOW);
//   analogWrite(pwmPin, 60);
// }

// void loop()
// {
//  Serial.print("Pulse:");
//  Serial.println(duration);
// //  duration = 0;
//  delay(100);
// }

// void EncoderInit()
// {
//  Direction = true;//default -> Forward
//  pinMode(encoder0pinB,INPUT);
//  attachInterrupt(0, wheelSpeed, CHANGE);
// }

// void wheelSpeed()
// {
//  int Lstate = digitalRead(encoder0pinA);
//  if((encoder0PinALast == LOW) && Lstate==HIGH)
//  {
//  int val = digitalRead(encoder0pinB);
//  if(val == LOW && Direction)
//  {
//  Direction = false; //Reverse
//  }
//  else if(val == HIGH && !Direction)
//  {
//  Direction = true; //Forward
//  }
//  }
//  encoder0PinALast = Lstate;

//  if(!Direction) duration++;
//  else duration--;
// } 














// #include <Encoder.h>

// const byte encoder0pinA = 2;//A pin -> the interrupt pin 0
// const byte encoder0pinB = 4;//B pin -> the digital pin 4

// const int forwardPin = 8;
// const int backwardPin = 12; 
// const int pwmPin = 10;

// // Connect the two encoder output pins to these pins.
// // For best performance, use pins with interrupt capability (e.g., 2 and 3 on Uno).
// Encoder myEnc(encoder0pinA, encoder0pinB); //myEnc(2, 3);

// long oldPosition  = -999;

// void setup() {
//   Serial.begin(9600);
//   Serial.println("Basic Encoder Test:");

//   pinMode(forwardPin, OUTPUT);
//   pinMode(backwardPin, OUTPUT);

//   digitalWrite(forwardPin, HIGH);
//   digitalWrite(backwardPin, LOW);
//   analogWrite(pwmPin, 70);
// }

// int loop_count = 0;

// void loop() {
//   // Read the current position
//   long newPosition = myEnc.read();

//   loop_count++;  
//   // Only print if the position has changed
//   if ((newPosition != oldPosition) || (loop_count % 3000 == 0)) {
//     oldPosition = newPosition;
//     Serial.print("Position: ");
//     Serial.println(newPosition);
//   }
// }
