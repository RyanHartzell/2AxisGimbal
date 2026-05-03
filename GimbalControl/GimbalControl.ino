#include<String.h>

/*
  Motor position variables
*/
// motor rest/starting positions
int sStart = 90;
int dcStart = 0; // TODO: check against final dc motor implementation
// motor end positions during scan
int sEnd = 0;
int dcEnd = 360; // TODO: check against final dc motor implementation

/*
  Mapping variables

  smaller scanDelay and scanInc theoretically means smoother scanning
  don't want to make scanDelay so small the camera doesn't get good data
  don't want to make scanInc so small or disproportionate to scanDelay that it takes ages to scan or get redundant camera data at similar points for no reason
*/
int sfovBuffer = 15; // adjust servo scan from [0, 90] to [0+buffer, 90-buffer]  // need at least some small buffer to avoid servo getting degree commands on the edge of its range and wrapping them to the other side
// TODO: can add a dcfovBuffer if desired (would need to update simpleScan())
int scanDelay = 42; // ms delay between motor movements during scan // 24 fps = 42 positions per second -> 1000 / 24 = 41.667 ms at each position
int sScanInc = 10; // move the servo 10 degrees at a time during scans
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

  // testSerial(); // listen for serial input, repeat back received messages in serial
  // testMotors(); // test serial, servo, and dc motor ///////// need to uncomment DCMotorControlLoop //////////

  // char* msg = readSerial();

  // loop_count++;


  // continually control the DC motor since it uses PID control to reach a target position
  DCMotorControlLoop();

  main_loop_count++;

  if (once) {
    DCMoveTo(180);
    once = false;
    Serial.println("Set to 90");
  }
  else if (main_loop_count < 8000 && twice) {
    stopMotor();
    twice = false;
    Serial.println("Stopped");
  }

  // if (loop_count < 100000) {
  //   if (loop_count % 5000 == 0) {
  //     Serial.print("Loop count = ");
  //     Serial.println(loop_count);
  //     if (loc_switch) {
  //       DCMoveTo(0);
  //       loc_switch = !loc_switch;
        
  //       Serial.println("DCMoveTo 0"); // DEBUG
  //     }
  //     else {
  //       DCMoveTo(90);
  //       loc_switch = !loc_switch;

  //       Serial.println("DCMoveTo 90"); // DEBUG
  //     }
  //   }
  // }
  // else {
  //   if (once) {
  //     DCMoveTo(0);
  //     once = false;
  //   }
  // }

  // // testing command
  // if (strcmp(msg, "T") == 0) {
  //   testMotors();
  // }
  // // simple scan command
  // else if (strcmp(msg, "S") == 0) {
  //   simpleScan();
  // }
  // // reset motors command
  // else if (strcmp(msg, "R") == 0) {
  //   resetMotors();
  // }

  // // TODO: servo/dc command reading might be sketchy, need to test

  // // set servo/elevation angle commands: i.e. "servo 90", "el 35"
  // else if (strcmp(msg, "servo", strlen("servo")) == 0) {
  //   servoMoveTo( atoi( msg[strlen("servo") : [strlen(msg)-1])); // convert rest of string to an int, send as motor command
  // }
  // else if (strcmp(msg, "el", strlen("el")) == 0) {
  //   servoMoveTo( atoi( msg[strlen("el") : [strlen(msg)-1]));
  // }
  // // set dc/azimuth angle commands: i.e. "dc 270", "az 10"
  // else if (strcmp(msg, "dc", strlen("dc")) == 0) {
  //   DCMoveTo( atoi( msg[strlen("dc") : [strlen(msg)-1]));
  // }
  // else if (strcmp(msg, "az", strlen("az")) == 0) {
  //   DCMoveTo( atoi( msg[strlen("az") : [strlen(msg)-1]));
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
      Serial.print("First loop servo degree: ");
      Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree -= sScanInc;
      delay(scanDelay);
    }
    sDegree += sScanInc; // set servo command back in range

    // TODO: may need to tune the dc scan increment and/or make a separate dc scan delay to ensure dc motor has enough time to get to the target position
    // move to the next az coordinate
    dcDegree += dcScanInc;
    DCMoveTo(dcDegree);
    delay(scanDelay);

    // scan the servo down at the current az coordinate
    while (sDegree <= sStart - sfovBuffer) {
      Serial.print("Second loop servo degree: ");
      Serial.println(sDegree);
      // move servo a small amount, pause to ensure a good camera read, continue scan
      servoMoveTo(sDegree);
      sDegree += sScanInc;
      delay(scanDelay);
    }

    // move to the next az coordinate
    dcDegree += sScanInc;
    DCMoveTo(dcDegree);
    delay(scanDelay);
    // if moved to 360 and returned to start, loop will end here so the scan of the starting position will not be repeated
  
  }

  // reset both motors to start position after the scan
  resetMotors();

}

void resetMotors() {

  // reset motors to center position
  servoMoveTo(sStart);
  DCResetMotor(); // hardcoded with the center position = 0/360

}