#ifndef TTNCREDENTIALS_H
#define TTNCREDENTIALS_H

// Enter your keys here

// NOTE: before uploading to the HTCC-AB02 run the Arduino: File>Examples> 'Examples for CubeCell-Board Plus (HTCC-AB02)' EEPROM > eeprom_clear

// This EUI must be in little-endian format For TTN issued EUIs the last bytes should be 0xD5, 0xB3, 0x70.
#define TTN_APPEUI {  }

// This should also be in little endian format
#define TTN_DEVEUI  {  }

// This key should be in big endian format
#define TTN_APPKEY {  }

#endif
