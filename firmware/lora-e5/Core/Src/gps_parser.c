#include "gps_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sys_app.h"

/*
   NMEA Data format:
   GGA 		Time, position and fix type data
   GLL 		Latitude, longitude, UTC time of position fix and status
   GSA    		GPS receiver operating mode, satellites used in the position
   solution, and DOP values GSV 		Number of GPS satellites in view
   satellite ID numbers, elevation, azimuth, & SNR values MSS
   Signal-to-noise ratio, signal strength, frequency, and bit rate from a
   radio-beacon receiver RMC 		Time, date, position, course and speed
   data VTG 		Course and speed information relative to the ground ZDA
   PPS timing message (synchronized to PPS)
   */

// static int checkChecksum(char* nmeaString) {
//     int len = strlen(nmeaString);
//     int i = 0;
//     int checksum = 0;
//     char calculatedChecksum[3];
//
//     // Calculate the checksum
//     for (i = 1; i < len; i++) {
//         if (nmeaString[i] == '*') {
//             break;
//         }
//         checksum ^= nmeaString[i];
//     }
//
//     // Convert the calculated checksum to hexadecimal
//     sAPP_LOG(TS_OFF, VLEVEL_M, calculatedChecksum, "%02X", checksum);
//
//     // Compare the calculated checksum with the one in the NMEA string
//     if (strcmp(&nmeaString[i + 1], calculatedChecksum) == 0) {
//         // remove the checksum from the string if passed for parsing
//         nmeaString[i] = '\0';
//         return 1;
//     } else {
//         return 0;
//     }
// }

// char sentence_cache[MAX_LINE_LEN];
// bool cacheLoaded = false;

void decode_nmea(char *sentence) {
	while (*sentence != '\0') {
		char line_arr[MAX_LINE_LEN]; /* Array to hold lines */
		int idx = 0;
		if (*sentence++ == '$') {
			do {
				line_arr[idx++] = *sentence++;
			} while (idx < MAX_LINE_LEN && *sentence != '\n' && *sentence != '$');

			if (strncmp(line_arr, "GPGGA", 5) == 0) {
				parse_gga(line_arr);
			} else if (strncmp(line_arr, "GPGLL", 5) == 0) {
				parse_gll(line_arr);
			} else if (strncmp(line_arr, "GPGSA", 5) == 0) {
				parse_gsa(line_arr);
			} else if (strncmp(line_arr, "GPGSV", 5) == 0) {
				parse_gsv(line_arr);
			} else if (strncmp(line_arr, "GPMSS", 5) == 0) {
				parse_mss(line_arr);
			} else if (strncmp(line_arr, "GPRMC", 5) == 0) {
				parse_rmc(line_arr);
			} else if (strncmp(line_arr, "GPVTG", 5) == 0) {
				parse_vtg(line_arr);
			} else if (strncmp(line_arr, "GPZDA", 5) == 0) {
				parse_zda(line_arr);
			} else {
				/* parse_fail */
				APP_LOG(TS_OFF, VLEVEL_M, "Parse failed!\n");
			}
		}
	}
	// int nlines = 0;
	// char *line_ptr;

	// // if(cacheLoaded){                        /* If there was a cache, make a
	// sentence*/
	// //     /* load cache and clear cache */
	// //     cacheLoaded = false;
	// //     /* append until '\n' or '$' */
	// // }

	// /* After dealing with the cache, load the rest of sentences */
	// while(sentence!= NULL && *sentence!='\0'){
	//     if (nlines>= MAX_LINES || (line_ptr =
	//     malloc(MAX_LINE_LEN))==NULL){return;} do{
	//         *line_ptr++ = *sentence++;
	//     }while(*sentence!='\0' && *sentence != '\n');
	//     strings_arr[nlines++] = line_ptr;
	// }

	// for(int i=0;i<nlines;i++){              /* While there is a sentence */
	//     // check valid sentence and Verify checksum
	//     char *thisline = strings_arr[i];
	//     if(*thisline++ == '$'){
	//         // Check type of sentence
	//         if (strncmp(thisline, "GPGGA", 5) == 0){
	//             parse_gga(thisline);
	//         } else if(strncmp(thisline, "GPGLL", 5) == 0){
	//             parse_gll(thisline);
	//         } else if(strncmp(thisline, "GPGSA", 5) == 0){
	//             parse_gsa(thisline);
	//         } else if(strncmp(thisline, "GPGSV", 5) == 0){
	//             parse_gsv(thisline);
	//         } else if(strncmp(thisline, "GPMSS", 5) == 0){
	//             parse_mss(thisline);
	//         } else if(strncmp(thisline, "GPRMC", 5) == 0){
	//             parse_rmc(thisline);
	//         } else if(strncmp(thisline, "GPVTG", 5) == 0){
	//             parse_vtg(thisline);
	//         } else if(strncmp(thisline, "GPZDA", 5) == 0){
	//             parse_zda(thisline);
	//         } else {
	//             /* parse_fail */
	//         }
	//     }
	//     free(thisline);
	// }
	//
	//     while(*sentence!='\0'){
	//        if(*sentence++ =='$'){
	//            char line[MAX_LINE_LEN];
	//            char *thisline;
	//            int index = 0;
	//            while(*sentence!='\0' && *sentence!='\n' && *sentence!='$'){
	//                line[index++] = *sentence++;
	//            }
	//            thisline = line;
	//            if (strncmp(thisline, "GPGGA", 5) == 0){
	//                parse_gga(thisline);
	//            } else if(strncmp(thisline, "GPGLL", 5) == 0){
	//                parse_gll(thisline);
	//            } else if(strncmp(thisline, "GPGSA", 5) == 0){
	//                parse_gsa(thisline);
	//            } else if(strncmp(thisline, "GPGSV", 5) == 0){
	//                parse_gsv(thisline);
	//            } else if(strncmp(thisline, "GPMSS", 5) == 0){
	//                parse_mss(thisline);
	//            } else if(strncmp(thisline, "GPRMC", 5) == 0){
	//                parse_rmc(thisline);
	//            } else if(strncmp(thisline, "GPVTG", 5) == 0){
	//                parse_vtg(thisline);
	//            } else if(strncmp(thisline, "GPZDA", 5) == 0){
	//                parse_zda(thisline);
	//            } else {
	//                    /* parse_fail */
	//                APP_LOG(TS_OFF, VLEVEL_M, "Parse failed!\n");
	//            }
	//        } else{
	//            sentence++;
	//        }
	//     }
}

void parse_gga(char *sentence) {
	gga_data data;
	// Decode the $GPGGA sentence
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	data.latitude = -1;
	while (p != NULL) {
		if (*p != '$' && *p != '*') {
			switch (index) {
				case 1:
					data.time = atof(p);
					break;
				case 2:
					data.latitude = atof(p);
					break;
				case 3:
					data.latitude_dir = *p;
					break;
				case 4:
					data.longitude = atof(p);
					break;
				case 5:
					data.longitude_dir = *p;
					break;
				case 6:
					data.fix_quality = atoi(p);
					break;
				case 7:
					data.satellites_used = atoi(p);
					break;
				case 8:
					data.HDOP = atof(p);
					break;
				case 9:
					data.altitude = atof(p);
					break;
				case 10:
					data.altitude_unit = *p;
					break;
				case 11:
					data.height_geoid = atof(p);
					break;
				case 12:
					data.height_geoid_unit = *p;
					break;
				case 13:
					data.age = atof(p);
					break;
				case 14:
					strcpy(data.station_id, p);
					break;
			}
			if (index < 14) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 14)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nTime (hhmmss.ss): %f \n", data.time);
	APP_LOG(TS_OFF, VLEVEL_M, "Latitude: %f %c\n", data.latitude,
			data.latitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Longitude: %f %c\n", data.longitude,
			data.longitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Fix quality: %d\n", data.fix_quality);
	APP_LOG(TS_OFF, VLEVEL_M, "Satellites used: %d\n", data.satellites_used);
	APP_LOG(TS_OFF, VLEVEL_M, "HDOP: %f\n", data.HDOP);
	APP_LOG(TS_OFF, VLEVEL_M, "Altitude: %f %c\n\n\n", data.altitude,
			data.altitude_unit);
}

void parse_gll(char *sentence) {
	gll_data data;
	// Decode the $GPGLL sentence
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '*' && *p != '$') {
			switch (index) {
				case 1:
					data.latitude = atof(p);
					break;
				case 2:
					data.latitude_dir = *p;
					break;
				case 3:
					data.longitude = atof(p);
					break;
				case 4:
					data.longitude_dir = *p;
					break;
				case 5:
					data.time = atof(p);
					break;
				case 6:
					data.status = *p;
					break;
				case 7:
					data.mode = *p;
					break;
			}
			if (index < 7) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 7)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nLatitude: %.4f %c\n", data.latitude,
			data.latitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Longitude: %.4f %c\n", data.longitude,
			data.longitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Time(hhmmss.ss): %.2f \n", data.time);
	APP_LOG(TS_OFF, VLEVEL_M, "Status: %c\n", data.status);
	APP_LOG(TS_OFF, VLEVEL_M, "Mode Indicator: %c\n\n\n", data.mode);
}

void parse_gsa(char *sentence) {
	gsa_data data;
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	int sat_index = 0;
	while (p != NULL) {
		if (*p != '*' && *p != '$') {
			// not empty
			switch (index) {
				case 1:
					data.mode = *p;
					break;
				case 2:
					data.fix_type = *p;
					break;
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
					if (atoi(p) != 0)
						// ID cant be zero unless data is missing
						data.sat_ids[sat_index++] = atoi(p);
					break;
				case 15:
					data.pdop = atof(p);
					break;
				case 16:
					data.hdop = atof(p);
					break;
				case 17:
					data.vdop = atof(p);
					break;
			}
			if (index < 17) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 17)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nMode: %c\n", data.mode);
	APP_LOG(TS_OFF, VLEVEL_M, "Fix Type: %d\n", data.fix_type);
	APP_LOG(TS_OFF, VLEVEL_M, "Satellites: ");
	for (int i = 0; i < sat_index; i++) {
		APP_LOG(TS_OFF, VLEVEL_M, "%d ", data.sat_ids[i]);
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\n");
	APP_LOG(TS_OFF, VLEVEL_M, "sat_index: %d\n", sat_index);
	APP_LOG(TS_OFF, VLEVEL_M, "PDOP: %.2f\n", data.pdop);
	APP_LOG(TS_OFF, VLEVEL_M, "HDOP: %.2f\n", data.hdop);
	APP_LOG(TS_OFF, VLEVEL_M, "VDOP: %.2f\n\n\n", data.vdop);
}

void parse_gsv(char *sentence) {
	gsv_data data;
	char *p = sentence;
	int sat_index = 0;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '*' && *p != '$') {
			// not empty
			switch (index) {
				case 1:
					data.total_msgs = atoi(p);
					break;
				case 2:
					data.msg_number = atoi(p);
					break;
				case 3:
					data.total_sats = atoi(p);
					break;
				case 4:
					data.satellites[sat_index].id = atoi(p);
					break;
				case 5:
					data.satellites[sat_index].elevation = atoi(p);
					break;
				case 6:
					data.satellites[sat_index].azimuth = atoi(p);
					break;
				case 7:
					data.satellites[sat_index].snr = atoi(p);
					sat_index++;
					APP_LOG(TS_OFF, VLEVEL_M, "I am here---\n");
					// index = 3;
					return;
					// break;
			}
			p = strchr(p, ',') + 1;
		} else if (*p == ',') {
			// nothing to process jump to next
			p = strchr(p, ',');
		} else if (*p == '*') {
			// at checksum
		} else if (*p == '$') {
			// new sentence creeped in, break to avoid conflicts with corrupt data
			break;
		}
		index++;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nTotal Messages: %d\n", data.total_msgs);
	APP_LOG(TS_OFF, VLEVEL_M, "Message Number: %d\n", data.msg_number);
	APP_LOG(TS_OFF, VLEVEL_M, "Total Satellites: %d\n", data.total_sats);
	APP_LOG(TS_OFF, VLEVEL_M, "Satellites:\n");
	for (int i = 0; i < sat_index; i++) {
		APP_LOG(TS_OFF, VLEVEL_M, "\tID: %d\n", data.satellites[i].id);
		APP_LOG(TS_OFF, VLEVEL_M, "\tElevation: %d\n",
				data.satellites[i].elevation);
		APP_LOG(TS_OFF, VLEVEL_M, "\tAzimuth: %d\n", data.satellites[i].azimuth);
		APP_LOG(TS_OFF, VLEVEL_M, "\tSNR: %d\n", data.satellites[i].snr);
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\n\n");
}

void parse_mss(char *sentence) {
	mss_data data;
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '$' && *p != '*') {
			switch (index) {
				case 1:
					data.signalStrength = atof(p);
					break;
				case 2:
					data.snr = atof(p);
					break;
				case 3:
					data.beacon_freq = atof(p);
					break;
				case 4:
					data.beacon_bitrate = atoi(p);
					break;
				case 5:
					data.channel_number = atoi(p);
					break;
			}
			if (index < 5) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 5)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nSignal strength is: %.2f\n",
			data.signalStrength);
	APP_LOG(TS_OFF, VLEVEL_M, "Signal to Noise ratio: %.2f\n", data.snr);
	APP_LOG(TS_OFF, VLEVEL_M, "Beacon frequency(KHz): %.2f\n", data.beacon_freq);
	APP_LOG(TS_OFF, VLEVEL_M, "Beacon bitrate (bits per second) : %d\n",
			data.beacon_bitrate);
	APP_LOG(TS_OFF, VLEVEL_M, "Channel number: %d\n\n\n", data.channel_number);
}

void parse_rmc(char *sentence) {
	rmc_data data;
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '$' && *p != '*') {
			switch (index) {
				case 1:
					data.time = atof(p);
					break;
				case 2:
					data.status = *p;
					break;
				case 3:
					data.latitude = atof(p);
					break;
				case 4:
					data.latitude_dir = *p;
					break;
				case 5:
					data.longitude = atof(p);
					break;
				case 6:
					data.longitude_dir = *p;
					break;
				case 7:
					data.speed = atof(p);
					break;
				case 8:
					data.heading = atof(p);
					break;
				case 9:
					data.date = atol(p);
					break;
				case 10:
					data.magnetic_variation = atof(p);
					break;
				case 11:
					data.direction = *p;
					break;
				case 12:
					data.mode = *p;
					break;
			}
			if (index < 12) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 12)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nTime UTC (hhmmss.sss): %.3f\n", data.time);
	APP_LOG(TS_OFF, VLEVEL_M, "Status %s\n",
			data.status == 'A' ? "Valid data." : "Invalid data.");
	APP_LOG(TS_OFF, VLEVEL_M, "Latitude: %.4f %c\n", data.latitude,
			data.latitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Longitude: %.4f %c\n", data.longitude,
			data.longitude_dir);
	APP_LOG(TS_OFF, VLEVEL_M, "Speed over ground: %.4f knots\n", data.speed);
	APP_LOG(TS_OFF, VLEVEL_M, "Course over ground: %.4f degrees\n", data.heading);
	APP_LOG(TS_OFF, VLEVEL_M, "Date (ddmmyy): %ld\n", data.date);
	APP_LOG(TS_OFF, VLEVEL_M, "Magnetic variation: %f degrees %c\n",
			data.magnetic_variation, data.direction);
	APP_LOG(TS_OFF, VLEVEL_M, "Mode: %c\n\n\n", data.mode);
}

void parse_vtg(char *sentence) {
	vtg_data data;
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '$' && *p != '*') {
			switch (index) {
				case 1:
					data.course1 = atof(p);
					break;
				case 2:
					data.reference1 = *p;
				case 3:
					data.course2 = atof(p);
					break;
				case 4:
					data.reference2 = *p;
				case 5:
					data.speed1 = atof(p);
					break;
				case 6:
					data.units1 = *p;
					break;
				case 7:
					data.speed2 = atof(p);
					break;
				case 8:
					data.units2 = *p;
					break;
				case 9:
					data.mode = *p;
					break;
			}
			if (index < 9) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 9)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nCourse1: %.4f degrees %c\n", data.course1,
			data.reference1);
	APP_LOG(TS_OFF, VLEVEL_M, "Course2: %.4f degrees %c\n", data.course2,
			data.reference2);
	APP_LOG(TS_OFF, VLEVEL_M, "Speed1: %.4f degrees %c\n", data.speed1,
			data.units1);
	APP_LOG(TS_OFF, VLEVEL_M, "Speed2: %.4f degrees %c\n", data.speed2,
			data.units2);
	APP_LOG(TS_OFF, VLEVEL_M, "Mode: %c\n\n\n", data.mode);
}

void parse_zda(char *sentence) {
	zda_data data;
	char *p = sentence;
	p = strchr(p, ',') + 1;
	int index = 1;
	while (p != NULL) {
		if (*p != '$' && *p != '*') {
			switch (index) {
				case 1:
					data.utcTime = atof(p);
					break;
				case 2:
					data.day = atoi(p);
				case 3:
					data.month = atoi(p);
					break;
				case 4:
					data.year = atoi(p);
				case 5:
					data.localZoneHr = atoi(p);
					break;
				case 6:
					data.localZoneMin = atoi(p);
					break;
			}
			if (index < 6) p = strchr(p, ',') + 1;
		} else if (*p == '$') {
			break;
		}
		if (index < 6)
			index++;
		else
			break;
	}
	APP_LOG(TS_OFF, VLEVEL_M, "\nTime(UTC): %.2f\n", data.utcTime);
	APP_LOG(TS_OFF, VLEVEL_M, "Day/Month/Year: %d/%d/%d\n", data.day, data.month,
			data.year);
	APP_LOG(TS_OFF, VLEVEL_M, "Local zone (hh:mm): %d:%d\n\n\n", data.localZoneHr,
			data.localZoneMin);
}
