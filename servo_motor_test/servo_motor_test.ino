/*
Servo spec sheet:
  https://www.electronicoscaldas.com/datasheet/MG90S_Tower-Pro.pdf?srsltid=AfmBOop7uN7O5-yJs3xmNVFiHsmYuWprzZhmXZeVfP-X7whEWiJldp4O
  (ignore how it says its -90 to 90 degrees, its actually 0 to 180 with Arduino's servo code library)
*/

#include <Servo.h>

int servoPin = 3;
bool demoed = false;

Servo Servo1;
void setup() {
  //Attach servo to proper pin
  Servo1.attach(servoPin);
}

void loop(){

  // rotates 180 degrees
  // based on Servo.h:
    // 0 is all the way to the left
    // 180 is all the way to the right

  /*
  Do once:
    Go all the way left
    Go to center
    Go all the way right
    Go 30 degrees from left
    Go back to center
  */
  if (!demoed) {
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

  demoed = true;

}
