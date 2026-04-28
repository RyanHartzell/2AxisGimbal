void testAll() {

  // read serial input in a loop and print received messages
  testSerial();

  // run each motor demo code once
  if (testDemos == false) {
    
    // servo demo
    // servoDemo();
    // delay(2000);
    
    // DC motor demo
    // TODO
    //delay(2000);

    // test simple scan
    simpleScan();
    delay(5000);
  }
  testDemos = true;
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

  servoMoveTo(0);
}

// TODO: DC motor test demo