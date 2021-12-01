#include "rg15.h"
#include "sensorcfg.h"
/*
  RG-15 Rain Sensor commands:
    A Reads accumulation data
    R Read all available data
    K Restart the rain sensor
    P Set to polling only mode (currenlty used)
    C Set to continuous mode, where data is sent when accumulation changes (default)
    H Force high resolution
    L Force low resolution
    I Force imperial
    M Force metric (default)
    S Revert to jumper configured values
    O Reset the accumulation counter
*/

/*
  RG-15 Possible Reset reasons:
     Reset N   N = Normal Power Up (default)
     Reset M   M = MCLR
     Reset W   W = Watchdog Timer Reset
     Reset O   O = Stack Overflow
     Reset U   U = Stack Underflow
     Reset B   B = Brownout (Low Voltage/disconnected)
     Reset D   D = Other
*/

String RG15_OP_MODE;
uint32_t MAX_COUNTER_RG15;

void sendCMDRG15(char cmd) {
  Serial1.println(cmd);
}

String getResponseRG15(uint TIME_OUT = 3000) {
  String readSerial;
  unsigned long startTime = millis();
  while(1){
    while (Serial1.available()) {
      if (Serial1.available() > 0) {
        char c = Serial1.read();  //gets one byte from serial buffer
        readSerial += c; //makes the string readString
      }
    }
    if (millis() - startTime > TIME_OUT) {
        // Serial.println("Timedout");
        break;
      }
  }
  return readSerial;
}

void printResponseRG15(uint TIME_OUT = 3000) {
  unsigned long startTime = millis();
  while (Serial1.available() > 0) {
    if (Serial1.available() > 0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      Serial.print(c); //makes the string readString
    }
    if (millis() - startTime > TIME_OUT) {
      // Serial.println("Timedout");
      break;
    }
  }
}

bool setModeTo(char m) {
  bool modeSet = false;
  String response;
  Serial1.println(m);
  while (!modeSet) {
    response = getResponseRG15(3000);
    if (response.length() > 0) {
      response.toUpperCase();
      if (response.charAt(0) == m) {
        Serial.print("Mode set successfully to: ");
        Serial.println(RG15_OP_MODE); //always true
        modeSet = true;
      } 
    }
  }
  return modeSet;
}

void clearTotalAccRG15(void) {
  sendCMDRG15('O'); // Clear any previous Total Acc
}

void hardResetRG15(void) {
  sendCMDRG15('K');   // send Hard-reset command
  String readSerial = getResponseRG15(10000);
  Serial.println(readSerial);
  clearTotalAccRG15(); // clear Total Acc
}

void debugRG15(void) { // Hard reset and set mode to Polling
  hardResetRG15();
  char m = RG15_OP_MODE.charAt(0);
  bool modeSet = setModeTo(m);
}

String readLastAvilableReading(void) {
  String readSerial;
  sendCMDRG15('R');   // get reading
  readSerial = getResponseRG15();
  return readSerial;
}

bool checkForErrors(String str) {
  if (str.charAt(0) != 'A') {
    Serial.println("Error Detected debugging...");
    return true;
  } else {
     Serial.println("No errors.");
    return false;
  }
}

bool isEventDetected(String str) {
  if (str.charAt(0) == ';' && str.indexOf("Event") > 0) {
    Serial.println("Event detected by RG-15");
    return true;
  } else {
    return false;
  }
}

String pollReadingFromRG15(void) {
  // Set mode to Polling
  char m = RG15_OP_MODE.charAt(0);
  bool modeSet = setModeTo(m);
  // Poll last available reading
  sendCMDRG15('R');   // get reading
  String lastAvailReading = readLastAvilableReading();
  lastAvailReading.trim();
  // check if it is Event detected string
  /* Event detected string format:
        ;----------------------------------------
        Event
        SW 1.000 2020.07.06
  */
  // Check for Event
  bool eventDetected = isEventDetected(lastAvailReading);
  if (eventDetected) {
    // read again
    lastAvailReading = readLastAvilableReading();
    lastAvailReading.trim();
  }
  bool errorDetected = checkForErrors(lastAvailReading);
  if (errorDetected){
    // debug and read again
    debugRG15();

    lastAvailReading = readLastAvilableReading();
    lastAvailReading.trim();
  }
  return lastAvailReading;
}

void setup_RG15(String mode = "Polling") { // Allowed strings: "P", "Polling", "C", "Continuous"
  // get mode
  RG15_OP_MODE = mode;
  char m = RG15_OP_MODE.charAt(0);
  // opens Serial1 port, sets data rate to 9600 bps
  Serial1.begin(9600);
  printResponseRG15(10000);
  hardResetRG15();
  //Manual mode set
  clearTotalAccRG15(); // No response
  setModeTo(m);
  // Check reset counter Max value
  MAX_COUNTER_RG15 = 1440; //(24*60*1000)/TX_INTERVAL
  Serial.print("the MAX_COUNTER_RG15 is: "); Serial.println(MAX_COUNTER_RG15);
}
