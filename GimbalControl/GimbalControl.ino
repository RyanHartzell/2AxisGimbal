// NEED TO TEST: serial communiation, servo control integrated into main function

// motor rest/starting positions
sStart = 90;
dcStart = 0; // TODO
// motor end positions during scan
sEnd = 0;
dcEnd = 360; // TODO

// mapping
/*
smaller scanDelay and scanInc theoretically means smoother scanning
don't want to make scanDelay so small the camera doesn't get good data
don't want to make scanInc so small or disproportionate to scanDelay that it takes ages to scan or get redundant camera data at similar points for no reason
*/
sfovBuffer = 15; // adjust servo scan from [0, 90] to [0+buffer, 90]
scanDelay = 100; // ms delay between motor movements during scan
scanInc = 10; // move the servo 10 degrees at a time during scans


void setup() {

  // Serial communication set up
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");

  // Servo set up
  Servo1.attach(servoPin);
}

void loop() {
  
  // comment out/delete when done testing
  testAll();

  // // wait for serial message to start scan
  // gotCmd = false
  // while (gotCmd == false) {
  //   gotCmd = readSerial();
  // }

  // // check which command was received
  // if (msg == 'M') {
  //   // start mapping
  //   simpleScan();
  // }

}

/*
  Complete a very simple scan of the entire environment
  Move camera up, rotate az slightly, move camera down, rotate az slightly, etc.
  Small delays between each motor movement
*/
void simpleScan() {

  /*
  Servo input commands:
    -90 is all the way to the left
    0 is straight up
    90 is all the way to the right
  
  Servo scan range:
    start at 90, level with horizon (all the way down/right)
    scan upwards to 0 + sfovBuffer (not quite all the way vertical)
  */

  // start the scan from the start positions for both motors
  resetMotors();

  sDegree = sStart;
  dcDegree = dcStart;

  // run the scan until the az motor has traveled the full 360 degrees
  while (dcDegree < dcEnd) {

    // scan the servo up at the current az coordinate
    while (sDegree >= sEnd + sfovBuffer) {
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree += scanInc;
      delay(scanDelay);
    }
    sDegree -= scanInc; // set servo command back in range

    // move to the next az coordinate
    dcDegree += scanInc;
    // TODO
    delay(scanDelay);

    // scan the servo down at the current az coordinate
    while (sDegree <= sStart) {
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree -= scanInc;
      delay(scanDelay);
    }

    // move to the next az coordinate
    dcDegree += scanInc;
    // TODO
    delay(scanDelay);
  }
}

void resetMotors() {

  // reset motors to center position
  servoMoveTo(sStart);
  // TODO: dc reset

}