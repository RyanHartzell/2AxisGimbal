boolean testDemos = false;

void testAll() {

  // read serial input in a loop and print received messages
  testSerial();

  // run each motor demo code once
  if testDemos == true {
    
    // servo demo
    servoDemo();
    delay(2000);
    
    // DC motor demo
    // TODO
    delay(2000);

    // test simple scan
    simpleScan();
    delay(5000);
  }
  testDemos = false;
}

void testSerial() {
  // read data from serial
  boolean readMsg = readSerial();
  // print received data
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
  Servo1.write(0);
  delay(1000);

  Servo1.write(90);
  delay(1000);

  Servo1.write(180);
  delay(1000);

  Servo1.write(30);
  delay(1000);

  Servo1.write(90);
}

// TODO: DC motor test demo