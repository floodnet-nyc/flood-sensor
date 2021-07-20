#include "maxbotix.h"
#include "functions.h"
#include "sensorcfg.h"
#include "softSerial.h"

softSerial softwareSerial(GPIO1 /*TX pin*/, GPIO2 /*RX pin*/);

// Maxbotix Ultrasonic variables
uint16_t readings_arr[20] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;

unsigned int sensorMode;
unsigned int sensor_sampling_rate;
unsigned int sensor_numberOfReadings;

void setup_maxbotix(unsigned int mode=2, unsigned int sampling_rate=250 , unsigned int numberOfReadings=5) {
  Serial.println("Setting up Maxbotix .... ");
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  softwareSerial.begin(9600);
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
  uint16_t distance;
  if (softwareSerial.available())
  {
    char serialbuffer[6] = {0};
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(20);            //Pin 4 ned to be pulled High for a minimum of 20 microseconds.
    digitalWrite(triggerPin, LOW);
    int i = 0;
    while (softwareSerial.available() && i < 6)
    {
      serialbuffer[i] = (char)softwareSerial.read();
      i++;
    }
    distance = (serialbuffer[1] - '0') * 1000 + (serialbuffer[2] - '0') * 100 + (serialbuffer[3] - '0') * 10 + (serialbuffer[4] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
  return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate=20, unsigned int sensor_numberOfReadings=5)
{
  uint16_t distance = 0;
  sensor_singleread();  // Ignore First read; First read in serial mode is incorrect
  Serial.println("Reading sensor using modes, entering measurements into an array...");
  for(int i=0;i<sensor_numberOfReadings;i++){
    readings_arr[i] = sensor_singleread();
    delay(sensor_sampling_rate);
  }
  Serial.println("Printing the array...");
  for(int i=0; i<n; i++){
    Serial.print(readings_arr[i]);Serial.print(" ");
  }
  Serial.println("");
  sort(readings_arr, n);
  Serial.println("Printing the sorted array...");
  for(int i=0; i<n; i++){
    Serial.print(readings_arr[i]);Serial.print(" ");
  }
  Serial.println("");

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
  for (int i=0; i<n; i++) {
    readings_arr[i] = 0;
  }
  Serial.println("");
  Serial.println("Printing the cleaned array...");
  for(int i=0; i<n; i++){
    Serial.print(readings_arr[i]);Serial.print(" ");
  }
  Serial.println("");
  return distance;
}
