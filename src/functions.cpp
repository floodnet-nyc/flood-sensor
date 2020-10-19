#include "functions.h"

void printHex2(unsigned v) {
  v &= 0xff;
  if (v < 16)
    Serial.print('0');
  Serial.print(v, HEX);
}

void swap(uint16_t  *p, uint16_t  *q) {
  int t;

  t = *p;
  *p = *q;
  *q = t;
}

void sort(uint16_t a[]) {
  int i, j, temp, n = 5;

  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - i - 1; j++) {
      if (a[j] > a[j + 1])
        swap(&a[j], &a[j + 1]);
    }
  }
}
