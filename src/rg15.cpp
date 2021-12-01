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

String getResponseRG15(void) {
  String readSerial;
  while (Serial1.available() > 0) {
    if (Serial1.available() > 0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      readSerial += c; //makes the string readString
    }
  }
  return readSerial;
}

bool setModeTo(char m) {
  bool setSucc = false;
  sendCMDRG15(m);
  String response = getResponseRG15();
  while (!setSucc) {
    if (response.length() > 0) {
      response.toUpperCase();
      if (response.charAt(0) == m) {
        setSucc = true;
      }
    }
  }
  return setSucc;
}

String hardResetRG15(void) {
  String readSerial;
  unsigned long startTime = millis();
  sendCMDRG15('R');   // get reading
  while (1) {           // timeout
    readSerial = getResponseRG15();
    if (millis() - startTime > 5000) {
      // Serial.println("Timedout");
      break;
    }
  }
  return readSerial;
}

void debugRG15(void) { // Hard reset and set mode to Polling
  char charinput;
  String readSerial;
  unsigned long startTime = millis();
  sendCMDRG15('K'); // Hard RESET
  while (1) {  // on reset
    readSerial = getResponseRG15();
    if (millis() - startTime > 5000) {
      // Serial.println("Timedout");
      break;
    }
  }
  if (setModeTo(RG15_OP_MODE.charAt(0))) {
    Serial.print("Mode set successfully to: ");
    Serial.println(RG15_OP_MODE); //always true
  }
}

void clearTotalAccRG15(void) {
  sendCMDRG15('O'); // Clear any previous Total Acc
}

void setup_RG15(String mode = "Polling") { // Allowed strings: "P", "Polling", "C", "Continuous"
  RG15_OP_MODE = mode;
  char charinput;
  unsigned long startTime = millis();
  Serial1.begin(9600); // opens Serial1 port, sets data rate to 9600 bps
  debugRG15();
  clearTotalAccRG15();
  // Check reset counter Max value
  MAX_COUNTER_RG15 = 1440; //(24*60*1000)/TX_INTERVAL
  Serial.print("the MAX_COUNTER_RG15 is: "); Serial.println(MAX_COUNTER_RG15);
}

String readLastAvilableReading(void) {
  String readSerial;
  unsigned long startTime = millis();
  sendCMDRG15('R');   // get reading
  while (1) {           // timeout
    readSerial = getResponseRG15();
    if (millis() - startTime > 1000) {
      // Serial.println("Timedout");
      break;
    }
  }
  return readSerial;
}

bool checkForErrors(String str) {
  if (str.charAt(0) != 'A') {
    return true;
  } else {
    return false;
  }
}

bool isEventDetected(String str){
  if (str.charAt(0)==';' && str.indexOf("Event") > 0){
    Serial.println("Event detected by RG-15");    
    return true;
  } else {
    return false;
  }
}

String pollReadingFromRG15(void) {
  // Set mode to Polling
  if (setModeTo(RG15_OP_MODE.charAt(0))) {
    Serial.print("Mode set successfully to: ");
    Serial.println(RG15_OP_MODE); //always true
  }
  // Poll last available reading
  String lastAvailReading = readLastAvilableReading();
  lastAvailReading.trim();
  // check if it is Event detected string
  /* Event detected string format:
        ;----------------------------------------
        Event
        SW 1.000 2020.07.06
  */
  // Check for Event
  if (isEventDetected(lastAvailReading)){
    // read again
    // Poll last available reading
    String lastAvailReading = readLastAvilableReading();
    lastAvailReading.trim();
  }
  // Check for Errors
  if (checkForErrors(lastAvailReading)){
    debugRG15();
  }
  return lastAvailReading;
}
