#include "maxbotix.h"
#include "functions.h"
#include "sensorcfg.h"

// Maxbotix Ultrasonic variables
uint16_t readings_arr[30] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;

unsigned int sensorMode;
unsigned int sensor_sampling_rate;
unsigned int sensor_numberOfReadings;

void setup_maxbotix(unsigned int mode=2, unsigned int sampling_rate=250 , unsigned int numberOfReadings=7) {
  Serial.println("Setting up Maxbotix .... ");
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);   //power line: now off
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);  //trigger line: now off
  Serial1.begin(9600);
  Serial.println("Sensor Settings:");
  sensorMode = mode;
  Serial.print("    Sensor mode: ");
  Serial.println(sensorMode);
  sensor_sampling_rate = sampling_rate;
  Serial.print("    Sensor sampling rate: ");
  Serial.println(sensor_sampling_rate);
  sensor_numberOfReadings = numberOfReadings;
  Serial.print("    Number of readings per measurement: ");
  Serial.println(sensor_numberOfReadings);
}

uint16_t sensor_singleread(void) {
  uint16_t distance=0;
  char buffer_RTT[6] = {};
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 (Trigger Pin) on Maxbotix ned to be pulled High for a minimum of 20 microseconds.
  digitalWrite(triggerPin, LOW);
  delay(150);
  if (Serial1.available() > 0) {
    do {
      if (Serial1.read() == 'R') {
        for (int i = 0; i < 5; i++) {
          buffer_RTT[i] = Serial1.read();
        }
      }
    } while (buffer_RTT == NULL);
    distance = (buffer_RTT[0] - '0') * 1000 + (buffer_RTT[1] - '0') * 100 + (buffer_RTT[2] - '0') * 10 + (buffer_RTT[3] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
  return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate = 20, unsigned int sensor_numberOfReadings = 5)
{
  uint16_t distance = 0;
  digitalWrite(Vext, LOW);
  sensor_singleread();
  delay(sensor_sampling_rate);
  for (int i = 0; i < sensor_numberOfReadings; i++) {
    readings_arr[i] = sensor_singleread();
    delay(sensor_sampling_rate);
  }
  digitalWrite(Vext, HIGH);
  sort(readings_arr, n);
  switch (sensorMode) {
    case 1:
      // Mean
      distance = mean(readings_arr, n, sensor_numberOfReadings);
      break;
    case 2:
      // Median
      distance = median(readings_arr, n, sensor_numberOfReadings);
      break;
    case 3:
      // Mode
      distance = mode(readings_arr, n, sensor_numberOfReadings);
      break;
    default:
      // Single Pulse-In single reading
      distance = sensor_singleread();
      break;
  }
  Serial.println("Cleaning measurements array...");
  for (int i = 0; i < n; i++) {
    readings_arr[i] = 0;
  }
  return distance;
}
