/*
TinyGPS++ - a small GPS library for Arduino providing universal NMEA parsing
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

#include "TinyGPSPlus.h"

#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "sys_app.h"

#define _GPRMCterm "GPRMC"
#define _GPGGAterm "GPGGA"
#define _GNRMCterm "GNRMC"
#define _GNGGAterm "GNGGA"

#define COMBINE(sentence_type, term_number)                                    \
  (((unsigned)(sentence_type) << 5) | term_number)

TinyGPSPlus_t *tGPSPlus;

/* Initializers */
void TinyGPSPlusInit(TinyGPSPlus_t *tinyGPS);
void TinyGPSCustomInit(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt, const char *_sentenceName,
                       int _termNumber);
void begin(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt, const char *_sentenceName,
           int _termNumber);
/* encoders and handlers */
bool TinyGPSEncode(TinyGPSPlus_t *tinyGPS, char c);
bool TinyGPSPlusEndOfTermHandler(TinyGPSPlus_t *tinyGPS);
/* Commit functions */
void TinyGPSLocationCommit(TinyGPSLocation_t *tinyGPSLocation);
void TinyGPSDateCommit(TinyGPSDate_t *tinyGPSDate);
void TinyGPSTimeCommit(TinyGPSTime_t *tinyGPSTime);
void TinyGPSSpeedCommit(TinyGPSSpeed_t *tinyGPSspeed);
void TinyGPSCourseCommit(TinyGPSCourse_t *tinyGPSCourse);
void TinyGPSAltitudeCommit(TinyGPSAltitude_t *tinyGPSAltitude);
void TinyGPSSatellitesCommit(TinyGPSInteger_t *tinyGPSsatellites);
void TinyGPSHdopCommit(TinyGPSHDOP_t *tinyGPSHdop);
void TinyGPSDecimalCommit(TinyGPSDecimal_t *tinyGPSDecimal);
void TinyGPSIntegerCommit(TinyGPSInteger_t *tinyGPSInteger);
void TinyGPSPlusCommit(TinyGPSPlus_t *tinyGPS);
void TinyGPSCustomCommit(TinyGPSPlusCustom_t *tinyGPSCustom);
/* get functions */
double getTinyGPSLocationLat(TinyGPSLocation_t *tinyGPSLocation);
double getTinyGPSLocationLng(TinyGPSLocation_t *tinyGPSLocation);
uint16_t getTinyGPSDateYear(TinyGPSDate_t *tinyGPSDate);
uint8_t getTinyGPSDateMonth(TinyGPSDate_t *tinyGPSDate);
uint8_t getTinyGPSDateDay(TinyGPSDate_t *tinyGPSDate);
uint8_t getTinyGPSTimeHour(TinyGPSTime_t *tinyGPSTime);
uint8_t getTinyGPSTimeMinute(TinyGPSTime_t *tinyGPSTime);
uint8_t getTinyGPSTimeSecond(TinyGPSTime_t *tinyGPSTime);
uint8_t getTinyGPSTimeCentisecond(TinyGPSTime_t *tinyGPSTime);
double getTinyGPSCourse(TinyGPSCourse_t *tinyGPSCourse);
/* Set Functions*/
void setTinyGPSLocationLatitude(TinyGPSLocation_t *tinyGPSLocation,
                                const char *term);
void setTinyGPSLocationLongitude(TinyGPSLocation_t *tinyGPSLocation,
                                 const char *term);
void setTinyGPSDate(TinyGPSDate_t *tinyGPSDate, const char *term);
void setTinyGPSTime(TinyGPSTime_t *tinyGPSTime, const char *term);
void setTinyGPSSpeed(TinyGPSSpeed_t *tinyGPSSpeed, const char *term);
void setTinyGPSCourse(TinyGPSCourse_t *tinyGPSCourse, const char *term);
void setTinyGPSDecimal(TinyGPSDecimal_t *tinyGPSDecimal, const char *term);
void setTinyGPSInteger(TinyGPSInteger_t *tinyGPSInteger, const char *term);
void setTinyGPSSatellites(TinyGPSInteger_t *tinyGPSsatellites,
                          const char *term);
void setTinyGPSHdop(TinyGPSHDOP_t *tinyGPSHdop, const char *term);
void setTinyGPSAltitude(TinyGPSAltitude_t *tinyGPSAltitude, const char *term);
void setTinyGPSCustom(TinyGPSPlusCustom_t *tinyGPSCustom, const char *term);
/* Utils */
int getFromHex(char a);
int32_t parseDecimal(const char *term);
void parseDegrees(const char *term, RawDegrees_t *deg);
double getDistanceBetween(double lat1, double long1, double lat2, double long2);
double getCourseTo(double lat1, double long1, double lat2, double long2);
const char *getCardinal(double course);
/* misc */
void TinyGPSPlusInsertCustom(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt, const char *sentenceName, int termNumber);

void TinyGPSPlusInit(TinyGPSPlus_t *tinyGPS) {
  tinyGPS->parity = 0;
  tinyGPS->isChecksumTerm = false;
  tinyGPS->curSentenceType = GPS_SENTENCE_OTHER;
  tinyGPS->curTermNumber = 0;
  tinyGPS->curTermOffset = 0;
  tinyGPS->sentenceHasFix = false;
  // tinyGPS->customElts = 0;
  // tinyGPS->customCandidates = 0;
  tinyGPS->encodedCharCount = 0;
  tinyGPS->sentencesWithFixCount = 0;
  tinyGPS->failedChecksumCount = 0;
  tinyGPS->passedChecksumCount = 0;
  tinyGPS->term[0] = '\0';
}

bool TinyGPSEncode(TinyGPSPlus_t *tinyGPS, char c) {
  tinyGPS->encodedCharCount++;

  switch (c) {
  case ',': // term terminators
    tinyGPS->parity ^= (uint8_t)c;

  case '\r':
  case '\n':
  case '*': {
    bool isValidSentence = false;
    if (tinyGPS->curTermOffset < sizeof(tinyGPS->term)) {
      tinyGPS->term[tinyGPS->curTermOffset] = 0;
      isValidSentence = TinyGPSPlusEndOfTermHandler(tinyGPS);
    }
    tinyGPS->curTermNumber++;
    tinyGPS->curTermOffset = 0;
    tinyGPS->isChecksumTerm = c == '*';
    return isValidSentence;
  } break;

  case '$': // sentence begin
    tinyGPS->curTermNumber = tinyGPS->curTermOffset = 0;
    tinyGPS->parity = 0;
    tinyGPS->curSentenceType = GPS_SENTENCE_OTHER;
    tinyGPS->isChecksumTerm = false;
    tinyGPS->sentenceHasFix = false;
    return false;

  default: // ordinary characters
    if (tinyGPS->curTermOffset < sizeof(tinyGPS->term) - 1)
      tinyGPS->term[tinyGPS->curTermOffset++] = c;
    if (!tinyGPS->isChecksumTerm)
      tinyGPS->parity ^= c;
    return false;
  }

  return false;
}

int getFromHex(char a) {
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

int32_t parseDecimal(const char *term) {
  bool negative = *term == '-';
  if (negative)
    ++term;
  int32_t ret = 100 * (int32_t)atol(term);
  while (isdigit((unsigned int)(*term)))
    ++term;
  if (*term == '.' && isdigit((unsigned int)(*term))) {
    ret += 10 * (term[1] - '0');
    if (isdigit((unsigned int)(term[2])))
      ret += term[2] - '0';
  }
  return negative ? -ret : ret;
}

void parseDegrees(const char *term, RawDegrees_t *deg) {
  uint32_t leftOfDecimal = (uint32_t)atol(term);
  uint16_t minutes = (uint16_t)(leftOfDecimal % 100);
  uint32_t multiplier = 10000000UL;
  uint32_t tenMillionthsOfMinutes = minutes * multiplier;

  deg->deg = (int16_t)(leftOfDecimal / 100);

  while (isdigit((unsigned int)*term))
    ++term;

  if (*term == '.')
    while (isdigit((unsigned int)*++term)) {
      multiplier /= 10;
      tenMillionthsOfMinutes += (*term - '0') * multiplier;
    }

  deg->billionths = (5 * tenMillionthsOfMinutes + 1) / 3;
  deg->negative = false;
}

bool TinyGPSPlusEndOfTermHandler(TinyGPSPlus_t *tinyGPS) {
  // If it's the checksum term, and the checksum checks out, commit
  if (tinyGPS->isChecksumTerm) {
    unsigned char checksum = 16 * getFromHex((unsigned int)&tinyGPS->term[0]) +
                             getFromHex((unsigned int)&tinyGPS->term[1]);
    if (checksum == tinyGPS->parity) {
      tinyGPS->passedChecksumCount++;
      if (tinyGPS->sentenceHasFix)
        tinyGPS->sentencesWithFixCount++;

      switch (tinyGPS->curSentenceType) {
      case GPS_SENTENCE_GPRMC:
        TinyGPSDateCommit(&tinyGPS->date);
        TinyGPSTimeCommit(&tinyGPS->time);
        if (tinyGPS->sentenceHasFix) {
          TinyGPSLocationCommit(&tinyGPS->location);
          TinyGPSSpeedCommit(&tinyGPS->speed);
          TinyGPSCourseCommit(&tinyGPS->course);
        }
        break;
      case GPS_SENTENCE_GPGGA:
        TinyGPSTimeCommit(&tinyGPS->time);
        if (tinyGPS->sentenceHasFix) {
          TinyGPSLocationCommit(&tinyGPS->location);
          TinyGPSAltitudeCommit(&tinyGPS->altitude);
        }
        TinyGPSSatellitesCommit(&tinyGPS->satellites);
        TinyGPSHdopCommit(&tinyGPS->hdop);
        break;
      }

      TinyGPSPlusCustom_t *p;
      // Commit all custom listeners of this sentence type
      for (p = tinyGPS->customCandidates;
           p != NULL && strcmp(p->sentenceName,
                               tinyGPS->customCandidates->sentenceName) == 0;
           p = p->next)
        strcpy(p->buffer, p->stagingBuffer);
      p->lastCommitTime = HAL_GetTick(); // get current time
      p->isValid = p->isUpdated = true;
      return true;
    }

    else {
      tinyGPS->failedChecksumCount++;
    }
    return false;
  }

  // the first term determines the sentence type
  if (tinyGPS->curTermNumber == 0) {
    if (!strcmp(tinyGPS->term, _GPRMCterm) ||
        !strcmp(tinyGPS->term, _GNRMCterm))
      tinyGPS->curSentenceType = GPS_SENTENCE_GPRMC;
    else if (!strcmp(tinyGPS->term, _GPGGAterm) ||
             !strcmp(tinyGPS->term, _GNGGAterm))
      tinyGPS->curSentenceType = GPS_SENTENCE_GPGGA;
    else
      tinyGPS->curSentenceType = GPS_SENTENCE_OTHER;

    // Any custom candidates of this sentence type?
    for (tinyGPS->customCandidates = tinyGPS->customElts;
         tinyGPS->customCandidates != NULL &&
         strcmp(tinyGPS->customCandidates->sentenceName, tinyGPS->term) < 0;
         tinyGPS->customCandidates = tinyGPS->customCandidates->next)
      ;
    if (tinyGPS->customCandidates != NULL &&
        strcmp(tinyGPS->customCandidates->sentenceName, tinyGPS->term) > 0)
      tinyGPS->customCandidates = NULL;

    return false;
  }

  if (tinyGPS->curSentenceType != GPS_SENTENCE_OTHER && tinyGPS->term[0])
    switch (COMBINE(tinyGPS->curSentenceType, tinyGPS->curTermNumber)) {
    case COMBINE(GPS_SENTENCE_GPRMC, 1): // Time in both sentences
    case COMBINE(GPS_SENTENCE_GPGGA, 1):
      setTinyGPSTime(&tinyGPS->time, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 2): // GPRMC validity
      tinyGPS->sentenceHasFix = tinyGPS->term[0] == 'A';
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 3): // Latitude
    case COMBINE(GPS_SENTENCE_GPGGA, 2):
      setTinyGPSLocationLatitude(&tinyGPS->location, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 4): // N/S
    case COMBINE(GPS_SENTENCE_GPGGA, 3):
      tinyGPS->location.rawNewLatData.negative = tinyGPS->term[0] == 'S';
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 5): // Longitude
    case COMBINE(GPS_SENTENCE_GPGGA, 4):
      setTinyGPSLocationLongitude(&tinyGPS->location, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 6): // E/W
    case COMBINE(GPS_SENTENCE_GPGGA, 5):
      tinyGPS->location.rawNewLngData.negative = tinyGPS->term[0] == 'W';
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 7): // Speed (GPRMC)
      setTinyGPSSpeed(&tinyGPS->speed, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 8): // Course (GPRMC)
      setTinyGPSCourse(&tinyGPS->course, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
      setTinyGPSDate(&tinyGPS->date, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
      tinyGPS->sentenceHasFix = tinyGPS->term[0] > '0';
      break;
    case COMBINE(GPS_SENTENCE_GPGGA, 7): // Satellites used (GPGGA)
      setTinyGPSSatellites(&tinyGPS->satellites, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPGGA, 8): // HDOP
      setTinyGPSHdop(&tinyGPS->hdop, tinyGPS->term);
      break;
    case COMBINE(GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
      setTinyGPSAltitude(&tinyGPS->altitude, tinyGPS->term);
      break;
    }

  // Set custom values as needed
  TinyGPSPlusCustom_t *p;
  for (p = tinyGPS->customCandidates;
       p != NULL &&
       strcmp(p->sentenceName, tinyGPS->customCandidates->sentenceName) == 0 &&
       p->termNumber <= tinyGPS->curTermNumber;
       p = p->next)
    if (p->termNumber == tinyGPS->curTermNumber)
      strncpy(p->stagingBuffer, tinyGPS->term, sizeof(p->stagingBuffer));

  return false;
}

double getDistanceBetween(double lat1, double long1, double lat2,
                          double long2) {
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6372795 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  double delta = radians(long1 - long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double slat1 = sin(lat1);
  double clat1 = cos(lat1);
  double slat2 = sin(lat2);
  double clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);
  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795;
}

double getCourseTo(double lat1, double long1, double lat2, double long2) {
  // returns course in degrees (North=0, West=270) from position 1 to position
  // 2, both specified as signed decimal-degrees latitude and longitude. Because
  // Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  double dlon = radians(long2 - long1);
  lat1 = radians(lat1);
  lat2 = radians(lat2);
  double a1 = sin(dlon) * cos(lat2);
  double a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0) {
    a2 +=  6.283;
  }
  return degrees(a2);
}

const char *getCardinal(double course) {
  static const char *directions[] = {"N",  "NNE", "NE", "ENE", "E",  "ESE",
                                     "SE", "SSE", "S",  "SSW", "SW", "WSW",
                                     "W",  "WNW", "NW", "NNW"};
  int direction = (int)((course + 11.25f) / 22.5f);
  return directions[direction % 16];
}

void TinyGPSLocationCommit(TinyGPSLocation_t *tinyGPSLocation) {
  tinyGPSLocation->rawLatData = tinyGPSLocation->rawNewLatData;
  tinyGPSLocation->rawLngData = tinyGPSLocation->rawNewLngData;
  tinyGPSLocation->lastCommitTime = HAL_GetTick(); // get current time
  tinyGPSLocation->isValid = tinyGPSLocation->isUpdated = true;
}

void setTinyGPSLocationLatitude(TinyGPSLocation_t *tinyGPSLocation,
                                const char *term) {
  parseDegrees(term, &tinyGPSLocation->rawNewLatData);
}
void setTinyGPSLocationLongitude(TinyGPSLocation_t *tinyGPSLocation,
                                 const char *term) {
  parseDegrees(term, &tinyGPSLocation->rawNewLngData);
}

double getTinyGPSLocationLat(TinyGPSLocation_t *tinyGPSLocation) {
  tinyGPSLocation->isUpdated = false;
  double ret = tinyGPSLocation->rawLatData.deg +
               tinyGPSLocation->rawLatData.billionths / 1000000000.0;
  return tinyGPSLocation->rawLatData.negative ? -ret : ret;
}

double getTinyGPSLocationLng(TinyGPSLocation_t *tinyGPSLocation) {
  tinyGPSLocation->isUpdated = false;
  double ret = tinyGPSLocation->rawLngData.deg +
               tinyGPSLocation->rawLngData.billionths / 1000000000.0;
  return tinyGPSLocation->rawLngData.negative ? -ret : ret;
}

// void TinyGPSSpeedCommit(TinyGPSSpeed_t *tinyGPSspeed) {
//   tinyGPSspeed->rawLatData = tinyGPSspeed->rawNewLatData;
//   tinyGPSspeed->rawLngData = tinyGPSspeed->rawNewLngData;
//   tinyGPSspeed->lastCommitTime = millis(); // get current time
//   tinyGPSspeed->isValid = tinyGPSspeed->isUpdated = true;
// }

void TinyGPSDateCommit(TinyGPSDate_t *tinyGPSDate) {
  tinyGPSDate->date = tinyGPSDate->newDate;
  tinyGPSDate->lastCommitTime = HAL_GetTick();
  tinyGPSDate->isValid = tinyGPSDate->isUpdated = true;
}
void TinyGPSTimeCommit(TinyGPSTime_t *tinyGPSTime) {
  tinyGPSTime->time = tinyGPSTime->newTime;
  tinyGPSTime->lastCommitTime = HAL_GetTick();
  tinyGPSTime->isValid = tinyGPSTime->isUpdated = true;
}

void TinyGPSTimeSetTime(TinyGPSTime_t *tinyGPSTime, const char *term) {
  tinyGPSTime->newTime = (uint32_t)parseDecimal(term);
}

void TinyGPSDateSetDate(TinyGPSDate_t *tinyGPSDate, const char *term) {
  (tinyGPSDate)->newDate = atol(term);
}

uint16_t getTinyGPSDateYear(TinyGPSDate_t *tinyGPSDate) {
  tinyGPSDate->isUpdated = false;
  uint16_t year = tinyGPSDate->date % 100;
  return year + 2000;
}

uint8_t getTinyGPSDateMonth(TinyGPSDate_t *tinyGPSDate) {
  tinyGPSDate->isUpdated = false;
  return (tinyGPSDate->date / 100) % 100;
}

uint8_t getTinyGPSDateDay(TinyGPSDate_t *tinyGPSDate) {
  tinyGPSDate->isUpdated = false;
  return tinyGPSDate->date / 10000;
}

uint8_t getTinyGPSTimeHour(TinyGPSTime_t *tinyGPSTime) {
  tinyGPSTime->isUpdated = false;
  return tinyGPSTime->time / 1000000;
}

uint8_t getTinyGPSTimeMinute(TinyGPSTime_t *tinyGPSTime) {
  tinyGPSTime->isUpdated = false;
  return (tinyGPSTime->time / 10000) % 100;
}

uint8_t getTinyGPSTimeSecond(TinyGPSTime_t *tinyGPSTime) {
  tinyGPSTime->isUpdated = false;
  return (tinyGPSTime->time / 100) % 100;
}

uint8_t getTinyGPSTimeCentisecond(TinyGPSTime_t *tinyGPSTime) {
  tinyGPSTime->isUpdated = false;
  return tinyGPSTime->time % 100;
}

void TinyGPSDecimalCommit(TinyGPSDecimal_t *tinyGPSDecimal) {
  tinyGPSDecimal->val = tinyGPSDecimal->newval;
  tinyGPSDecimal->lastCommitTime =HAL_GetTick();
  tinyGPSDecimal->isValid = tinyGPSDecimal->isUpdated = true;
}

void TinyGPSDecimalSetDecimal(TinyGPSDecimal_t *tinyGPSDecimal,
                              const char *term) {
  tinyGPSDecimal->newval = parseDecimal(term);
}

void TinyGPSIntegerCommit(TinyGPSInteger_t *tinyGPSInteger) {
  tinyGPSInteger->val = tinyGPSInteger->newval;
  tinyGPSInteger->lastCommitTime = HAL_GetTick();
  tinyGPSInteger->isValid = tinyGPSInteger->isUpdated = true;
}

void TinyGPSIntegerSetInteger(TinyGPSInteger_t *tinyGPSInteger,
                              const char *term) {
  tinyGPSInteger->newval = atol(term);
}

void TinyGPSCustomCommit(TinyGPSPlusCustom_t *tinyGPSCustom) {
  strcpy(tinyGPSCustom->buffer, tinyGPSCustom->stagingBuffer);
  tinyGPSCustom->lastCommitTime = HAL_GetTick();
  tinyGPSCustom->isValid = tinyGPSCustom->isUpdated = true;
}

void TinyGPSCustomSet(TinyGPSPlusCustom_t *tinyGPSCustom, const char *term) {
  strncpy(tinyGPSCustom->stagingBuffer, term,
          sizeof(tinyGPSCustom->stagingBuffer));
}

void TinyGPSPlusInsertCustom(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt,
                             const char *sentenceName, int termNumber) {
  TinyGPSPlusCustom_t **ppelt;
  for (ppelt = &gps->customElts; *ppelt != NULL; ppelt = &(*ppelt)->next) {
    int cmp = strcmp(sentenceName, (*ppelt)->sentenceName);
    if (cmp < 0 || (cmp == 0 && termNumber < (*ppelt)->termNumber))
      break;
  }
  pElt->next = *ppelt;
  *ppelt = pElt;
}

void TinyGPSCustomInit(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt, const char *_sentenceName,
                       int _termNumber) {
  begin(gps, pElt, _sentenceName, _termNumber);
}

void begin(TinyGPSPlus_t *gps, TinyGPSPlusCustom_t *pElt,
           const char *_sentenceName, int _termNumber) {
  pElt->lastCommitTime = 0;
  pElt->isUpdated = pElt->isValid = false;
  pElt->sentenceName = _sentenceName;
  pElt->termNumber = _termNumber;
  memset(pElt->stagingBuffer, '\0', sizeof(pElt->stagingBuffer));
  memset(pElt->buffer, '\0', sizeof(pElt->buffer));

  // Insert this item into the GPS tree
  TinyGPSPlusInsertCustom(gps, pElt, _sentenceName, _termNumber);
}

// void TinyGPSCourseCommit(TinyGPSCourse_t *tinyGPSCourse) {}
