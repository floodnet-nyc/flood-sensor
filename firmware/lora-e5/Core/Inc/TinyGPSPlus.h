/*

   ------- Adapted to C -------
TinyGPSPlus - a small GPS library for Arduino providing universal NMEA parsing
Based on work by and "distanceBetween" and "courseTo" courtesy of Maarten
Lamers. Suggestion to add satellites, courseTo(), and cardinal() by Matt Monson.
Location precision improvements suggested by Wayne Holder.
Copyright (C) 2008-2013 Mikal Hart
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __TinyGPSPlus_h

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>
#include <stdio.h>
#include <stdbool.h>

#define _GPS_VERSION "1.0.3" // software version of this library
#define _GPS_MPH_PER_KNOT 1.15077945
#define _GPS_MPS_PER_KNOT 0.51444444
#define _GPS_KMPH_PER_KNOT 1.852
#define _GPS_MILES_PER_METER 0.00062137112
#define _GPS_KM_PER_METER 0.001
#define _GPS_FEET_PER_METER 3.2808399
#define _GPS_MAX_FIELD_SIZE 15

/* ----------------- Data types --------------------- */

enum _enumGPSSentence_t {
  GPS_SENTENCE_GPGGA = 0,
  GPS_SENTENCE_GPRMC,
  GPS_SENTENCE_OTHER
};
typedef enum _enumGPSSentence_t enumGPSSentence_t;

typedef struct RawDegrees_t{
  uint16_t deg;
  uint32_t billionths;
  bool negative;
} RawDegrees_t;

typedef struct TinyGPSLocation_t{
  bool isValid;
  bool isUpdated;
  uint32_t age;
  RawDegrees_t rawLatData, rawLngData, rawNewLatData, rawNewLngData;
  uint32_t lastCommitTime;
} TinyGPSLocation_t;

typedef struct TinyGPSDate_t {
  bool isValid;
  bool isUpdated;
  uint32_t date, newDate, lastCommitTime;
  uint32_t age;
  uint16_t year;
  uint8_t month, day;
} TinyGPSDate_t;

typedef struct TinyGPSTime_t {
  bool isValid;
  bool isUpdated;
  uint32_t time, newTime;
  uint32_t lastCommitTime;
  uint32_t age;
  uint8_t hour;
  uint8_t minute, second, centisecond;
} TinyGPSTime_t;

typedef struct TinyGPSDecimal_t {
  bool isValid;
  bool isUpdated;
  uint32_t lastCommitTime;
  uint32_t age;
  int32_t val, newval;
} TinyGPSDecimal_t;

typedef struct TinyGPSInteger_t {
  bool isValid;
  bool isUpdated;
  uint32_t lastCommitTime;
  uint32_t age;
  uint32_t val, newval;
} TinyGPSInteger_t;

typedef struct TinyGPSSpeed_t {
  double knots;
  double mph;
  double mps;
  double kmph;
} TinyGPSSpeed_t;

typedef struct TinyGPSCourse_t {
  double deg;
} TinyGPSCourse_t;

typedef struct TinyGPSAltitude_t {
  double meters;
  double miles;
  double kilometers;
  double feet;
} TinyGPSAltitude_t;

typedef struct TinyGPSHDOP_t {
  double hdop;
} TinyGPSHDOP_t;

static const char *libraryVersion = _GPS_VERSION;

typedef struct TinyGPSPlusCustom_t {
  bool isValid;
  bool isUpdated;
  uint32_t age;
  char stagingBuffer[_GPS_MAX_FIELD_SIZE + 1];
  char buffer[_GPS_MAX_FIELD_SIZE + 1];
  unsigned long lastCommitTime;
  const char *sentenceName;
  int termNumber;
  struct TinyGPSPlusCustom_t *next;
}TinyGPSPlusCustom_t;

typedef struct TinyGPSPlus_t {
  // members
  TinyGPSLocation_t location;
  TinyGPSDate_t date;
  TinyGPSTime_t time;
  TinyGPSSpeed_t speed;
  TinyGPSCourse_t course;
  TinyGPSAltitude_t altitude;
  TinyGPSInteger_t satellites;
  TinyGPSHDOP_t hdop;

  // parsing state variables
  uint8_t parity;
  bool isChecksumTerm;
  char term[_GPS_MAX_FIELD_SIZE];
  uint8_t curSentenceType;
  uint8_t curTermNumber;
  uint8_t curTermOffset;
  bool sentenceHasFix;

  // statistics
  uint32_t encodedCharCount;
  uint32_t sentencesWithFixCount;
  uint32_t failedChecksumCount;
  uint32_t passedChecksumCount;

  TinyGPSPlusCustom_t *customElts;
  TinyGPSPlusCustom_t *customCandidates;
} TinyGPSPlus_t;


#ifdef __cplusplus
}
#endif

#endif
