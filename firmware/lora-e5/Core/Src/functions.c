#include "functions.h"

extern volatile uint16_t readings_arr[30];

static void swap(volatile uint16_t *p, volatile uint16_t *q) {
  int t = *p;
  *p = *q;
  *q = t;
}

static int partition(int lowIndex, int highIndex) {
  int pivotElement = readings_arr[highIndex];
  int i = (lowIndex - 1);
  for (int j = lowIndex; j <= highIndex - 1; j++) {
    if (readings_arr[j] <= pivotElement) {
      i++;
      swap(&readings_arr[i], &readings_arr[j]);
    }
  }
  swap(&readings_arr[i + 1], &readings_arr[highIndex]);
  return (i + 1);
}

static void quickSort(int lowIndex, int highIndex) {
  if (lowIndex < highIndex) {
    int pivot = partition(lowIndex, highIndex);
    quickSort(lowIndex, pivot - 1);
    quickSort(pivot + 1, highIndex);
  }
}

uint16_t mean(unsigned int number_of_samples){
    uint16_t readings_sum = 0;
    for (int i=0; i<number_of_samples; i++){
        readings_sum = readings_sum + readings_arr[i];
    }
    return readings_sum/number_of_samples;
}

uint16_t median(unsigned int number_of_samples){
    quickSort(0, number_of_samples-1);
    if (number_of_samples%2 != 0){
        return readings_arr[(number_of_samples + 1) / 2 - 1];
    }
    else {
        return (readings_arr[number_of_samples/2 -1]+ readings_arr[number_of_samples/2])/2;
    }
}

uint16_t mode_(unsigned int number_of_samples){
    int counter = 1;
    int max = 0;
    uint16_t mode_ = readings_arr[0];
    for (int i = 0; i < number_of_samples - 1; i++){
        if ( readings_arr[i] == readings_arr[i+1] ){
            counter++;
            if ( counter > max ){
                max = counter;
                mode_ = readings_arr[i];
            }
        } else
            counter = 1;         
    }
    return mode_;
}
