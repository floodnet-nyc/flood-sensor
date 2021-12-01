#ifndef RG15_H
#define RG15_H

#define bufferSize 144

#include <Arduino.h>

void setup_RG15(String RG15_OP_MODE); // Allowed strings: "P", "Polling", "C", "Continuous"
String pollReadingFromRG15(void);
void clearTotalAccRG15(void);

#endif