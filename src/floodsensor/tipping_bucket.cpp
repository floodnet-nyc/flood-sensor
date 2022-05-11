#include "tipping_bucket.h"
#include "lorawan.h"
// #include <Arduino.h>
// #ifdef __asr650x__
// #include "innerWdt.h"
// #endif

uint16_t level = 0;

uint32_t running_total_tip = 0;
uint32_t running_total_non_tip = 0;

uint16_t avg_over_tip = 10;
uint16_t avg_over_non_tip = 20;

uint32_t level_idx_tip = 0;
uint32_t level_idx_non_tip = 0;

bool tipping = false;

uint32_t tip_count = 0;

uint32_t TIME_DELTA_MS;
uint16_t ADC_READ_DELTA_MS;

void init_tipping_bucket(uint16_t PRECIP_AVG_DELTA_S, uint16_t TIME_BETWEEN_ADC_READ_MS){

  tipping = false;
  tip_count = 0;
  running_total_tip = 0;
  running_total_non_tip = 0;
  level_idx_tip = 0;
  level_idx_non_tip = 0;
  TIME_DELTA_MS = PRECIP_AVG_DELTA_S * 1000;
  ADC_READ_DELTA_MS = TIME_BETWEEN_ADC_READ_MS;
}


uint16_t tip_count_looper(void){
  
  uint32_t st_time = millis();
  innerWdtEnable(false);
  while(millis() - st_time <= TIME_DELTA_MS){
    // Read analog value from ADC1 pin (pin 1 - bottom right of AB02 if USB is up)
    level = analogRead(ADC1);
    running_total_tip += level;
    running_total_non_tip   += level;
    level_idx_tip += 1;
    level_idx_non_tip += 1;
    
    if (level_idx_tip == avg_over_tip && running_total_tip / avg_over_tip < 100){
       tipping = true;
    }

    if (level_idx_non_tip == avg_over_non_tip && running_total_non_tip / avg_over_non_tip > 1000){
      if (tipping){
        tip_count += 1;
        Serial.println(tip_count);
      }
      tipping = false;
    }
    
    if (level_idx_tip == avg_over_tip){
      level_idx_tip = 0;
      running_total_tip = 0;
    }

    if (level_idx_non_tip == avg_over_non_tip){
      level_idx_non_tip = 0;
      running_total_non_tip = 0;
    }
    feedInnerWdt();
    delay(ADC_READ_DELTA_MS);
  }
  CySysWdtDisable();
  return tip_count;
}