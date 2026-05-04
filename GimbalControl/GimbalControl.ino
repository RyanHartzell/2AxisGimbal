#include<String.h>

/*
  Motor position variables
*/
// motor rest/starting positions
int sStart = 90;
int dcStart = 0;
// motor end positions during scan
int sEnd = 0;
int dcEnd = 360;

/*
  Mapping variables

  smaller scanDelay and scanInc theoretically means smoother scanning
  don't want to make scanDelay so small the camera doesn't get good data
  don't want to make scanInc so small or disproportionate to scanDelay that it takes ages to scan or get redundant camera data at similar points for no reason
*/
int sfovBuffer = 15; // adjust servo scan from [0, 90] to [0+buffer, 90-buffer]  // need at least some small buffer to avoid servo getting degree commands on the edge of its range and wrapping them to the other side
// TODO: can add a dcfovBuffer if desired (would need to update simpleScan())
int scanDelay = 50; // ms delay between motor movements during scan ////// 24 fps = 42 positions per second -> 1000 / 24 = 41.667 ms at each position
int sScanInc = 20; // move the servo 10 degrees at a time during scans
int dcScanInc = 30; // move the dc motor 30 degrees at a time during scans


/*
  Do all set up for serial, servo, and dc motor
*/
void setup() {

  setupSerial();
  setupServo();
  setupDCMotor();

  // set motors to start position
  resetMotors();

}

int main_loop_count = 0;
bool loc_switch = false;
bool once = true;
bool twice = true;

void loop() {

  //// uncomment for testing //// comment everything else in loop
  // testSerial(); // listen for serial input, repeat back received messages in serial
  // testMotors(); // test servo and dc motor
  // delay(10000);
  ///////////////////////////////

  char* msg = readSerial();
  showNewData();

  // continually control the DC motor since it uses PID control to reach a target position
  DCMotorControlLoop();

  // testing command
  if (strcmp(msg, "T") == 0) {
    Serial.println("Testing motors.");
    testMotors();
  }
  // simple scan command
  else if (strcmp(msg, "M") == 0) {
    Serial.println("Running simpleScan.");
    simpleScan();
  }
  // reset motors command
  else if (strcmp(msg, "R") == 0) {
    Serial.println("Reseting motors.");
    resetMotors();
  }
  // stop motors command
  else if (strcmp(msg, "S") == 0) {
    Serial.println("Stopping motors.");
    stopMotor();
  }

  // TODO: servo/dc command reading might be sketchy, need to test

  // set servo/elevation angle commands: i.e. "servo 90", "el 35"
  else if (strncmp(msg, "servo", strlen("servo")) == 0) {
    servoMoveTo( atoi(msg + strlen("servo")) ); // convert rest of string to an int, send as motor command
  }
  else if (strncmp(msg, "el", strlen("el")) == 0) {
    servoMoveTo( atoi(msg + strlen("el")) );
  }
  // set dc/azimuth angle commands: i.e. "dc 270", "az 10"
  else if (strncmp(msg, "dc", strlen("dc")) == 0) {
    DCMoveTo( atoi(msg + strlen("dc")) );
  }
  else if (strncmp(msg, "az", strlen("az")) == 0) {
    DCMoveTo( atoi(msg + strlen("az")) );
  }
  // print dc encoder count command
  else if (strcmp(msg, "enc") == 0) {
    Serial.print("DC encoder counts = ");
    Serial.println(getEncoderCount());
  }

}

/*
  Run DCMotorControlLoop() until the input time has passed
  Used in place of a delay so DC motor control will still work

  @param waitTime [int] time to 'delay' in milliseconds
*/
void waitUntilReady(int waitTime) {

  unsigned long startT = millis();

  while ((millis() - startT) < waitTime) { // while the time passed is less than waitTime
    DCMotorControlLoop();
  }

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

  DC motor input commands:
    0 is starting position
    90 is a quarter rotation in the positive direction
    360 is the starting position, completing a full rotation

  DC scan range:
    start at 0
    end at 360, scanning a full forwards rotation and returning to the starting position
  */

  // start the scan from the start positions for both motors
  resetMotors();

  int sDegree = sStart;
  int dcDegree = dcStart;

  // run the scan until the az motor has traveled the full 360 degrees
  while (dcDegree < dcEnd) {

    // scan the servo up at the current az coordinate
    while (sDegree >= sEnd + sfovBuffer) {
      // Serial.print("First loop servo degree: ");
      // Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree -= sScanInc;
      // delay(scanDelay);
      waitUntilReady(scanDelay);
    }
    sDegree += sScanInc; // set servo command back in range

    // TODO: may need to tune the dc scan increment and/or make a separate dc scan delay to ensure dc motor has enough time to get to the target position
    // move to the next az coordinate
    dcDegree += dcScanInc;

    Serial.print("SS first loop dcDegree = ");
    Serial.println(dcDegree);

    DCMoveTo(dcDegree);
    // delay(scanDelay);
    waitUntilReady(scanDelay);

    // scan the servo down at the current az coordinate
    while (sDegree <= sStart - sfovBuffer) {
      // Serial.print("Second loop servo degree: ");
      // Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree += sScanInc;
      // delay(scanDelay);
      waitUntilReady(scanDelay);
    }

    // move to the next az coordinate
    dcDegree += sScanInc;

    Serial.print("SS second loop dcDegree = ");
    Serial.println(dcDegree);

    DCMoveTo(dcDegree);
    // delay(scanDelay);
    waitUntilReady(scanDelay);
    // if moved to 360 and returned to start, loop will end here so the scan of the starting position will not be repeated
  
  }

  // reset both motors to start position after the scan
  resetMotors();

}

void resetMotors() {

  // reset motors to center position
  servoMoveTo(sStart);
  DCResetMotor(); // hardcoded with reset position 0 / 360
  waitUntilReady(1000); // should be long enough for DC motor to finish resetting
  resetEncoder(); // reset dc encoders

}