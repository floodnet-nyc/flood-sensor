#ifndef _GPS_PARSER_H
#define _GPS_PARSER_H

#define MAX_SATELLITES 12

#define MAX_LINE_LEN 120
#define MAX_LINES 10

typedef struct gga_data {
	double time;
	double latitude;
	char latitude_dir;
	double longitude;
	char longitude_dir;
	int fix_quality;
	int satellites_used;
	double HDOP;
	double altitude;
	char altitude_unit;
	double height_geoid;
	char height_geoid_unit;
	double age;
	char station_id[4];
}gga_data;

typedef struct gll_data {
	double latitude;
	char latitude_dir;
	double longitude;
	char longitude_dir;
	double time;
	char status;
	char mode;
}gll_data;

typedef struct gsa_data {
	char mode;
	char fix_type;
	int sat_ids[MAX_SATELLITES];
	float pdop;
	float hdop;
	float vdop;
}gsa_data;

struct gsv_satellite {
	int id;
	int elevation;
	int azimuth;
	int snr;
};

typedef struct gsv_data {
	int total_msgs;
	int msg_number;
	int total_sats;
	struct gsv_satellite satellites[12];
}gsv_data;

typedef struct mss_data {     
	float signalStrength; 
	float snr; 
	float beacon_freq;
	int beacon_bitrate;
	int channel_number;
}mss_data;

typedef struct rmc_data {    
	double time;             
	char status;             
	double latitude;         
	char latitude_dir;  
	double longitude;        
	char longitude_dir; 
	double speed;            
	double heading;          
	long date;                
	double magnetic_variation;
	char direction;          
	char mode;               
}rmc_data;

typedef struct vtg_data {
	double course1;
	char reference1;
	double course2;
	char reference2;
	double speed1;
	char units1;
	double speed2;
	char units2;
	char mode;
}vtg_data;

typedef struct zda_data {
	double utcTime;
	int day;
	int month;
	int year;
	int localZoneHr;
	int localZoneMin;
}zda_data;

void isValid(char sentence[]);
void decode_nmea(char *sentence);
void parse_gga(char *sentence);
void parse_gll(char *sentence);
void parse_gsa(char *sentence);
void parse_gsv(char *sentence);
void parse_mss(char *sentence);
void parse_rmc(char *sentence);
void parse_vtg(char *sentence);
void parse_zda(char *sentence);

#endif
