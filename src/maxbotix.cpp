#include "maxbotix.h"
#include "functions.h"
#include "sensorcfg.h"

// Maxbotix Ultrasonic variables
uint16_t readings_arr[30] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;

unsigned int sensorMode;
unsigned int sensor_sampling_rate;
unsigned int sensor_numberOfReadings;

void setup_maxbotix(unsigned int mode=2, unsigned int sampling_rate=250 , unsigned int numberOfReadings=15) {
    Serial.println("Setting up Maxbotix .... ");
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
    int distance =0;
    char serialbuffer[4];
    int index = 0;
    char rc;
    Serial1.flush();
    boolean newData = false;
    while (newData == false) {
        if (Serial1.available())
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
            newData = true;
        }
    }
    if (newData){
        distance = (serialbuffer[0] - '0') * 1000 + (serialbuffer[1] - '0') * 100 + (serialbuffer[2] - '0') * 10 + (serialbuffer[3] - '0');
        Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
    }
    return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate=20, unsigned int sensor_numberOfReadings=5)
{
    uint16_t distance = 0;
    Serial.println("Reading sensor using modes, entering measurements into an array...");
    Serial.println("VEXT Low: Sensor Powered up");
    digitalWrite(Vext, LOW);
    digitalWrite(triggerPin, HIGH);
    Serial.println("Trigger Pin High: Sensor reading...");
    for(int i=0;i<sensor_numberOfReadings;i++){
        readings_arr[i] = sensor_singleread();
        delay(sensor_sampling_rate);
    }
    digitalWrite(triggerPin, LOW);
    Serial.println("Trigger Pin Low: Sensor stopped.");
    Serial.println("VEXT High: Sensor turned off");
    digitalWrite(Vext, HIGH);
    
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
