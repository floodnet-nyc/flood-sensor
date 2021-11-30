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

uint32_t MAX_COUNTER_RG15;

String getResponseRG15(void) {
  String readSerial;
  while (Serial1.available() > 0) {
    if (Serial1.available() > 0) {
      char c = Serial1.read();  //gets one byte from serial buffer
      readSerial += c; //makes the string readString
    }
  }
  if (readSerial.length() > 0) {
    Serial.println(readSerial); //for debug, see what was received
  }
  return readSerial;
}


void setup_RG15(String RG15_OP_MODE = "Polling") { // Allowed strings: "P", "Polling", "C", "Continuous"
  char charinput;
  unsigned long startTime = millis();
  Serial1.begin(9600); // opens Serial1 port, sets data rate to 9600 bps
  while (1) {  // on reset
    while (Serial1.available() > 0) {
      charinput = Serial1.read();
      Serial.print(charinput); // If you want to see what is in the buffer
    }
    if (millis() - startTime > 10000) {
      Serial.println("Timedout");
      break;
    }
  }
  bool modeSet = false;
  String response;
  char m = RG15_OP_MODE.charAt(0);
  Serial1.println('O'); // every setup clear Total Acc
  // set mode only after seeing data
  Serial1.println(m);
  while (!modeSet) {
    response = getResponseRG15();
    if (response.length() > 0) {
      response.toUpperCase();
      if (response.charAt(0) == m) {
        Serial.print("Mode set successfully to: "); Serial.println(RG15_OP_MODE);
        modeSet = true;
      }
    }
  }
  // Check reset counter Max value
  MAX_COUNTER_RG15 = 1440; //(24*60*1000)/TX_INTERVAL
  Serial.print("the MAX_COUNTER_RG15 is: ");Serial.println(MAX_COUNTER_RG15);
}

String pollReadingFromRG15(void) {
  String readSerial;
  char charinput;
  unsigned long startTime = millis();
  Serial1.println('R');
  while (1) {           // timeout
      while (Serial1.available()) {
        if (Serial1.available() > 0) {
          char c = Serial1.read();  //gets one byte from serial buffer
          readSerial += c; //makes the string readString
        }
      }
    if (millis() - startTime > 3000) {
      Serial.println("Timedout");
      break;
    }
  }
  if (readSerial.length() > 0) {
    Serial.println(readSerial);
  }
  return readSerial;
}