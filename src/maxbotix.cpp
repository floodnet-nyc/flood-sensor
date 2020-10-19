#include "maxbotix.h"
#include "functions.h"
#include "sensorcfg.h"

// Maxbotix Ultrasonic variables

uint16_t median_arr[5];
unsigned int sensorMode;
unsigned int sensor_sampling_rate;
unsigned int sensor_numberOfReadings;

void setup_maxbotix(unsigned int mode = 5, unsigned int sampling_rate = 20 , unsigned int numberOfReadings = 5 ) {
                     // wait for a second
  Serial.println("Setting up Maxbotix ........ ");
  Serial.println("Sensor Settings:");
  sensorMode = mode;
  Serial.print("  Sensor mode: ");
  Serial.println(sensorMode);
  sensor_sampling_rate = sampling_rate;
  Serial.print("  Sensor sampling rate: ");
  Serial.println(sensor_sampling_rate);
  sensor_numberOfReadings = numberOfReadings;
  Serial.print("  Sensor no of readings per measurement: ");
  Serial.println(sensor_numberOfReadings);
  pinMode(triggerPin, OUTPUT);
  pinMode(readPin, INPUT);
}

uint16_t sensor_singleread(void) {
  uint16_t distance = 0;
  uint16_t duration = 0;
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds.
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(readPin, HIGH);
  distance = duration;
  return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode=5, unsigned int sensor_sampling_rate=20, unsigned int sensor_numberOfReadings=5)
{
  uint16_t distance = 0;
  // Currently single sensor mode: Median of 5 readings

  // Switch Sensor mode needs to be implemented

  for(int i=0; i<5; i++){
    distance = sensor_singleread();
    Serial.print("Reading "); Serial.print(i+1  + " :");
    Serial.print(distance);
    Serial.println(" mm");
    median_arr[i] = distance;
    delay(sensor_sampling_rate);
    //sleep(sensor_sampling_rate);
  }

  sort(median_arr);
  int n = (sensor_numberOfReadings + 1) / 2 - 1;
  // pick the median
  distance = median_arr[n];
  return distance;
}
