#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Arduino.h>
#include <bits/stdc++.h>

void printHex2(unsigned v);
uint16_t mean(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings);
uint16_t median(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings);
uint16_t mode(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings);

#endif
