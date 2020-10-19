/*  ---------------------------------------------------------------------------
 *  This Example uses Maxbotix Ultrasonic Sensor and Feather m0 LoRa module
 *  to read the depth and send the distance and battery level over the
 *  LoRa network.
 *  
 *  LoRa: This uses OTAA (Over-the-air activation), where registered keys for
 *  DEVEUI, APPEUI and APPKEY are required. Multiple devices can use the
 *  same AppEUI, but each device has its own DevEUI and AppKey.
 *  
 *  Maxbotix sensor mode: This code uses the median of 5 readings as an averaging
 *  method for stable readings.
 *  
 *  SD Card Logging: For Debugging and local storage, every significant event is
 *  logged onto the SD Card using sdfatlib library
 *  sdfatlib library can be found at: https://github.com/jbeynon/sdfatlib
 *  
 *  Deepsleep: using Adafruits sleepydog library
 *  Adafruit_sleepydog can be found at: https://github.com/adafruit/Adafruit_SleepyDog
 *  
 *  DutyCycle: the duty cycle is controlled by the sleep time. Which is currently
 *  stored in the variable TX_INTERVAL
 *  
 *  RTC: This has a real time clock which keep tracks of time even when the
 *  MCU is off. Using this RTC SD Card loggings keep track of timestamps.
 *  
 *  Connections:
 *    VBATPIN A7 -> batterylevel pin
 *    GPIO 10    -> cardSelect
 *    GPIO 11    -> Maxbotix readPin
 *    GPIO 12    -> Maxbotix triggerPin
 *  
 */


#include <Floodsense_sensor.h>

void setup() {
  delay(5000);
  Serial.begin(9600);
  Serial.println("Starting");

  setup_maxbotix(5, 20, 5);  // sensor mode 5, 20 microseconds sampling rate, 5 readings per measurement
  setup_featherWing();        // set up SD card and RTC. Sets date and time everytime compiled.
  lmicsetup(300);             // packet interval 300 seconds
}

void loop() {
  lorawan_runloop_once(); 
}
