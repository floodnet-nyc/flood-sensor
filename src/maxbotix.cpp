#include "maxbotix.h"
#include "functions.h"
#include "sensorcfg.h"

// Maxbotix Ultrasonic variables
uint16_t readings_arr[30] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;

unsigned int SENSOR_MODE;
unsigned int SENSOR_SAMPLING_RATE;
unsigned int SENSOR_NUMBER_OF_READINGS;

void setup_maxbotix(unsigned int mode=2, unsigned int sampling_rate=250 , unsigned int numberOfReadings=7) {
  Serial.println("Setting up Maxbotix .... ");
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);   //power line: now off
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);  //trigger line: now off
  Serial1.begin(9600);
  Serial.println("Sensor Settings:");
  SENSOR_MODE = mode;
  Serial.print("    Sensor mode: ");
  Serial.println(SENSOR_MODE);
  SENSOR_SAMPLING_RATE = sampling_rate;
  Serial.print("    Sensor sampling rate: ");
  Serial.println(SENSOR_SAMPLING_RATE);
  SENSOR_NUMBER_OF_READINGS = numberOfReadings;
  Serial.print("    Number of readings per measurement: ");
  Serial.println(SENSOR_NUMBER_OF_READINGS);
}

uint16_t sensor_singleread(void) {
  int distance = 0;
  char serialbuffer[4];
  int index = 0;
  char rc;
  Serial1.flush();
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(20); 
  digitalWrite(triggerPin, LOW);
  delay(150);
  boolean newData = false;
  while (newData == false) {
    if (Serial1.available()>0)
    {
      char rc = Serial1.read();
      if (rc == 'R')
      {
        while (index < 3)
        {
          if (Serial1.available())
          {
            serialbuffer[index] = Serial1.read();
            index++;
          }
        }
      }
      distance = (serialbuffer[0] - '0') * 1000 + (serialbuffer[1] - '0') * 100 + (serialbuffer[2] - '0') * 10 + (serialbuffer[3] - '0');
      if (distance>0){
        newData = true;
      } 
    }
  }
  if (newData) {
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm"); 
  } 
  return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate = 20, unsigned int sensor_numberOfReadings = 5)
{
  uint16_t distance = 0;
  digitalWrite(Vext, LOW);
  sensor_singleread();
  for (int i = 0; i < sensor_numberOfReadings + 1; i++) {
    if (i>0){ //1st reading is R from "Maxbotix Corp"
      readings_arr[i] = sensor_singleread();
    }
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