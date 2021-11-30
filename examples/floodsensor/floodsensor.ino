/*  ---------------------------------------------------------------------------
 *  This Example uses Maxbotix Ultrasonic Sensor and Feather m0 LoRa module
 *  to read the depth and send the distance and battery level over the
 *  LoRa network.
 *
 *  LoRa: This uses OTAA (Over-the-air activation), where registered keys for
 *  DEVEUI, APPEUI and APPKEY are required. Multiple devices can use the
 *  same AppEUI, but each device has its own DevEUI and AppKey.
 *
 *  Maxbotix sensor mode: The following are available sensor modes
 *      Sensor mode	  Statistical method
 *          1	            Mean
 *          2	            Median
 *          3	            Mode
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
 *    GPIO 11    -> Maxbotix readPin (Pin 2: Pulse Width Output)
 *    GPIO 12    -> Maxbotix triggerPin (Pin 4- Ranging Start/Stop)
 *
 */

#define USE_RG15

#include <Floodsense_sensor.h>

void setup() {
  Serial.begin(115200);
  /* Sensors */
  // Ultrasonic Range Finder, MaxBotix MB-7389
  #ifdef USE_MAXBOTIX
    setup_maxbotix(2,150,7);  //mode, sampling rate, number of readings
  #endif
  // Rainguage, RG-15
  #ifdef USE_RG15
    setup_RG15("Polling");       // Allowed strings: "P", "Polling", "C", "Continuous"
  #endif

  /* LoRaWAN */
  setup_lorawan(60);   // duty cycle in seconds 
}

void loop() {
  lorawan_runloop_once();
}
