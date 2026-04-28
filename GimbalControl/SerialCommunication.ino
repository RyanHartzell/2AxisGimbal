/*
  Read in several characters from serial
*/
boolean readSerial() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial.available() > 0 && newData == false) { // loop while the serial is avaialable until message is finished
        rc = Serial.read();

        if (rc != endMarker) { // overwrites the last character if the message is too long
            msg[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            msg[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
            return true;
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