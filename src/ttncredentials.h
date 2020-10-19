#ifndef TTNCREDENTIALS_H
#define TTNCREDENTIALS_H

// This EUI must be in little-endian format For TTN issued EUIs the last bytes should be 0xD5, 0xB3, 0x70.
#define TTN_APPEUI {  }

// This should also be in little endian format
#define TTN_DEVEUI  {  }

// This key should be in big endian format
#define TTN_APPKEY {  }

#endif
