/*
Servo spec sheet:
  https://www.electronicoscaldas.com/datasheet/MG90S_Tower-Pro.pdf?srsltid=AfmBOop7uN7O5-yJs3xmNVFiHsmYuWprzZhmXZeVfP-X7whEWiJldp4O
  (ignore how it says its -90 to 90 degrees, its actually 0 to 180 with Arduino's servo code library)

Under the hood servo control with Servo.h:
  0 is all the way to the left
  90 is straight up
  180 is all the way to the right

Input commands:
  -90 is all the way to the left
  0 is straight up
  90 is all the way to the right
*/

#include <Servo.h>

int servoPin = 3;
Servo Servo1;

/*
  Moves the servo motor to the specified degree
  @param degree in range [-90, 90]
*/
void servoMoveTo(degree) {

  // adjust input degree from [-90, 90] to [0, 180]
  degree += 90
  degree = degree % 180 // wrap around if needed

  // send the command to the servo
  Servo1.write(degree);

}