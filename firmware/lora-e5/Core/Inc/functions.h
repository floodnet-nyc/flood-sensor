#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

uint16_t mean(unsigned int number_of_samples);
uint16_t median(unsigned int number_of_samples);
uint16_t mode_(unsigned int number_of_samples);

#ifdef __cplusplus
}
#endif

#endif
