int pin1 = 12;
int pwmPin = 10;
int pin2 = 8;

//boolean to switch direction
bool directionState;

void setup() {

//define pins
pinMode(pin1, OUTPUT);
pinMode(pwmPin, OUTPUT);
pinMode(pin2, OUTPUT);

directionState = true;
}

void loop() {

//change direction every loop()
directionState = !directionState;

analogWrite(pwmPin, 50); // 0-255 range
//turn left
digitalWrite(pin1, 1);
digitalWrite(pin2, 1);
delay(2000);
//turn right
digitalWrite(pin1, 1);
digitalWrite(pin2, 1);
delay(2000);
//stop
digitalWrite(pin1, 0);
digitalWrite(pin2, 0);
delay(2000);
//speed up
analogWrite(pwmPin, 128);
delay(2000);
//slow down
analogWrite(pwmPin, 50);
delay(2000);

//Alt stop
analogWrite(pwmPin, 0);

delay(2000);
}