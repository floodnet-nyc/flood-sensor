#ifndef TIPPING_BUCKET_H
#define TIPPING_BUCKET_H

#pragma once

#include <Arduino.h>

void init_tipping_bucket(uint16_t PRECIP_AVG_DELTA_S, uint16_t TIME_BETWEEN_ADC_READ_MS);

uint16_t tip_count_looper(void);

#endif