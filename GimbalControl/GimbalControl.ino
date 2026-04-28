// NEED TO TEST: serial communiation, servo control integrated into main function

// Servo variables
#include<Servo.h>
int servoPin = 3;
Servo Servo1;

// Serial communication variables
const byte numChars = 32;
char msg[numChars];   // an array to store the received data
boolean newData = false;

// Testing variables
boolean testDemos = false;

// motor rest/starting positions
int sStart = 90;
int dcStart = 0; // TODO
// motor end positions during scan
int sEnd = 0;
int dcEnd = 360; // TODO

// mapping
/*
smaller scanDelay and scanInc theoretically means smoother scanning
don't want to make scanDelay so small the camera doesn't get good data
don't want to make scanInc so small or disproportionate to scanDelay that it takes ages to scan or get redundant camera data at similar points for no reason
*/
int sfovBuffer = 15; // adjust servo scan from [0, 90] to [0+buffer, 90-buffer]
int scanDelay = 42; // ms delay between motor movements during scan // 24 fps = 42 positions per second -> 1000 / 24 = 41.667 ms at each position
int scanInc = 10; // move the servo 10 degrees at a time during scans

// track if first time running loop()
boolean first_loop = true;


void setup() {

  // Serial communication set up
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");

  // Servo set up
  Servo1.attach(servoPin);
}

void loop() {
  
  // set motors to start position
  if (first_loop == true) {
    resetMotors();
    first_loop = false;
  }

  //boolean readMsg = readSerial();

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

  int sDegree = sStart;
  int dcDegree = dcStart;

  // run the scan until the az motor has traveled the full 360 degrees
  while (dcDegree < dcEnd) {

    // scan the servo up at the current az coordinate
    while (sDegree >= sEnd + sfovBuffer) {
      Serial.print("First loop servo degree: ");
      Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree -= scanInc;
      delay(scanDelay);
    }
    sDegree += scanInc; // set servo command back in range

    // move to the next az coordinate
    dcDegree += scanInc;
    // TODO: move dc motor
    delay(scanDelay);

    // scan the servo down at the current az coordinate
    while (sDegree <= sStart - sfovBuffer) {
      Serial.print("Second loop servo degree: ");
      Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree += scanInc;
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