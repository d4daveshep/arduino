const byte serialBufSize = 12;
char receivedChars[serialBufSize];

boolean newSerialDataReceived = false;

void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
}

void loop() {
  
  readSerialWithStartEndMarkers();
  showNewData();
}

void readSerialWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newSerialDataReceived == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= serialBufSize) {
          ndx = serialBufSize - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newSerialDataReceived = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  if (newSerialDataReceived == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    float target = atof(receivedChars);
    Serial.println(target);
    newSerialDataReceived = false;
  }
}



