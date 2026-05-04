const byte numChars = 32;
//// not done // char msg[numChars] declared in GimbalControl.ino
char msg[numChars];   // an array to store the data received from serial
boolean newData = false;

/*
  Set up the serial communication
*/
void setupSerial() {
  Serial.begin(9600);

  // small delay to let serial connect
  delay(700);

  Serial.println();
  Serial.println();
  Serial.println("<Arduino is ready>");
  Serial.println();
  Serial.println();

}

/*
  Read in several characters from serial
*/
char* readSerial() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 ) { //&& newData == false) { // loop while the serial is avaialable until message is finished
        rc = Serial.read();

        if (rc != endMarker) { // overwrites the message buffer if the message is too long
            msg[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = 0;
            }
        }
        else {
            msg[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
            return msg;
        }
    }
}

/*
  Print data most recently received from serial (if it exists)
*/
void showNewData() {
    if (newData == true) { // only print data if it was just received
        Serial.print("Received: ");
        Serial.println(msg);
        newData = false;
    }
}