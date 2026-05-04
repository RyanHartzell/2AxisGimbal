void testMotors() {
    
  // servo demo
  servoDemo();
  delay(2000);
  
  // DC motor demo
  DCMotorDemo();
  
}

/*
  Read data from the serial and repeat the received message back
*/
void testSerial() {
  
  readSerial();
  showNewData();
  
}

void servoDemo() {
  /*
  Do once:
    Go all the way left
    Go to center
    Go all the way right
    Go 30 degrees from left
    Go back to center
  */

  /*
  servoMoveTo() takes angle inputs [-90, 90]
  servo.write() takes angle inputs [0, 180]
  */

  servoMoveTo(-90);
  delay(1000);

  servoMoveTo(0);
  delay(1000);

  servoMoveTo(90);
  delay(1000);

  servoMoveTo(-30);
  delay(1000);

  servoMoveTo(90);
}

/*
  Test the DC motor control
  (DCMotorControlLoop must be called in the main loop() function to work)

  1. Move to 0 degrees
  2. Move to 180 degrees
  3. Move to 360 degrees (same as 0)
  4. Move to -90 degrees (go backwards)
  5. Move to 0 degrees
  
  Two second delay between each command
*/
void DCMotorDemo() {

  DCMoveTo(0);
  waitUntilReady(3000);
  Serial.print("Encoder counts after 0 = ");
  Serial.println(getEncoderCount());

  DCMoveTo(180);
  waitUntilReady(3000);
  Serial.print("Encoder counts after 180 = ");
  Serial.println(getEncoderCount());

  DCMoveTo(360);
  waitUntilReady(3000);
  Serial.print("Encoder counts after 360 = ");
  Serial.println(getEncoderCount());

  DCMoveTo(-90);
  waitUntilReady(3000);
  Serial.print("Encoder counts after -90 = ");
  Serial.println(getEncoderCount());

  DCMoveTo(0);
  waitUntilReady(3000);
  Serial.print("Encoder counts after 0 = ");
  Serial.println(getEncoderCount());

}