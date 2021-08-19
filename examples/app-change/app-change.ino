#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <EEPROM.h>
#ifdef __asr650x__
#include "innerWdt.h"
#endif

/*
   set LoraWan_RGB to Active,the RGB active in loraWan
   RGB red means sending;
   RGB purple means joined done;
   RGB blue means RxWindow1;
   RGB yellow means RxWindow2;
   RGB green means received done;
*/

// Enter your keys here

// This EUI must be in big-endian format
#define TTN_APPEUI {  }

// This should also be in big endian format
#define TTN_DEVEUI  {  }

// This key should be in big endian format
#define TTN_APPKEY {  }

/* Watchdog timer */
bool autoFeed = false;

/* EEPROM params */
const int selectionAddress = 8; // Single char (y/n) in ASCII
const int init_Address = 9;
const int newAppAddress = 12;    // 12 to 19
const int newAppKeyAddress = 21;  //21 to 37
const int newDevEUIAddress = 40;
const int defaultAppEUIAddress = 50; // 50 to 57
const int defaultAppKeyAddress = 58; // 58 to 73
const int defaultDevEUIAddress = 75; // 75 to 82
char useOrigApp = 'y';

/* OTAA para*/
/* OTAA para*/
uint8_t devEui[] = TTN_DEVEUI;
uint8_t appEui[] = TTN_APPEUI;
uint8_t appKey[] = TTN_APPKEY;
//uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x04, 0x3C, 0xE6 };
//uint8_t appEui[] = { 0x12, 0x14, 0x25, 0x41, 0x23, 0x12, 0x31, 0x23 };
//uint8_t appKey[] = { 0x10, 0x4D, 0xA1, 0x0A, 0x20, 0x1F, 0xE2, 0x1A, 0x24, 0xB6, 0x91, 0xF9, 0xD1, 0xBA, 0x98, 0xFE };

/* ABP para*/
/* ABP para*/
uint8_t nwkSKey[] = { };
uint8_t appSKey[] = { };
uint32_t devAddr =  ( uint32_t )0x0000;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass = LORAWAN_CLASS;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle = 30000;

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 2;
/*!
  Number of trials to transmit the frame, if the LoRaMAC layer did not
  receive an acknowledgment. The MAC performs a datarate adaptation,
  according to the LoRaWAN Specification V1.0.2, chapter 18.4, according
  to the following table:

  Transmission nb | Data Rate
  ----------------|-----------
  1 (first)       | DR
  2               | DR
  3               | max(DR-1,0)
  4               | max(DR-1,0)
  5               | max(DR-2,0)
  6               | max(DR-2,0)
  7               | max(DR-3,0)
  8               | max(DR-3,0)

  Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
  the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials = 4;

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
    appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
    if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
    if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
    for example, if use REGION_CN470,
    the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */
  appDataSize = 4;
  appData[0] = 0x00;
  appData[1] = 0x01;
  appData[2] = 0x02;
  appData[3] = 0x03;
}

//downlink data handle function example
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n", mcpsIndication->RxSlot ? "RXWIN2" : "RXWIN1", mcpsIndication->BufferSize, mcpsIndication->Port);
  Serial.print("+REV DATA:");
  for (uint8_t i = 0; i < mcpsIndication->BufferSize; i++)
  {
    Serial.printf("%02X", mcpsIndication->Buffer[i]);
  }
  Serial.println();
  uint32_t color = mcpsIndication->Buffer[0] << 16 | mcpsIndication->Buffer[1] << 8 | mcpsIndication->Buffer[2];
#if (LoraWan_RGB==1)
  turnOnRGB(color, 5000);
  turnOffRGB();
#endif
  // process received downlink
  /*
     Downlink Packet format:
        |   oper     | Duty Cycle in seconds  | Sensor Mode | Sampling Rate  |  Number of readings per measurement  |
        |  1 byte    |       2 bytes          |    1 byte   |    2 bytes     |        1 byte                        |
  */
  // set UPDATE_CONFIG to true
  //UPDATE_CONFIG = true;
  switch (mcpsIndication->Buffer[0]) {
    case 0x4D:
      /* Change sensor Mode*/
      //ModifySensorSettings(mcpsIndication);
      //UPDATE_CONFIG == true;
      Serial.println("Modify Sensor Settings");
      break;
    case 0x41:
      /* Change APPEUI*/
      Serial.println("Change APPEUI");
      if (mcpsIndication->BufferSize == 33) // 33: 0x41, AppEUI, AppKey, DevEUI
      {
        // Modify AppEUI
        ModifyKeys(mcpsIndication);
        // Reset
        innerWdtEnable(false);
        delay(5000); //Wait until the MCU resets
        //        // Load keys
        //        EEPROM.begin(512);
        //        LoadNewKeys();  // load keys
        //        EEPROM.end();
        //        Serial.println("Keys loaded...Start OTAA again...");
        //        // take care of keys
        //#if(LORAWAN_DEVEUI_AUTO)
        //        LoRaWAN.generateDeveuiByChipID();
        //#endif
        //#if(AT_SUPPORT)
        //        Serial.println("Getting device parameters...");
        //        getDevParam();
        //#endif
        //        printDevParam();
        //        deviceState = DEVICE_STATE_JOIN;
      } else {
        Serial.println("Invalid AppEUI and AppKey size!");
      }
      break;
    default:
      Serial.println("Invalid downlink format!");
      break;
  }
}

void ModifyKeys(McpsIndication_t *mcpsIndication) {
  EEPROM.begin(512);
  for (int i = 0; i < 8; ++i)
  {
    EEPROM.write(newAppAddress + i, byte(mcpsIndication->Buffer[i + 1])); //big-endian
  }
  for (int i = 0; i < 16; ++i)
  {
    EEPROM.write(newAppKeyAddress + i, byte(mcpsIndication->Buffer[i + 9])); //big-endian
  }
  for (int i = 0; i < 8; ++i)
  {
    EEPROM.write(newDevEUIAddress + i, byte(mcpsIndication->Buffer[i + 25])); //big-endian
  }
  useOrigApp = 'n'; // Use new AppEUI on restart
  EEPROM.write(selectionAddress, byte('n')); // Save the choice to Flash Memory
  // restart
  EEPROM.end();
  delay(500);
}

void LoadNewKeys(void) {
  bool foundNewAppEUI;
  if (int(EEPROM.read(newAppAddress)) != 0) {
    // check if first byte is a zero
    foundNewAppEUI = true;
  } else {
    // all zeroes is default key
    foundNewAppEUI = false;
  }
  if (foundNewAppEUI) {
    // load new keys
    Serial.print("New AppEUI found.");
    for (int i = 0; i < 8; ++i)
    {
      appEui[i] = (uint8_t) EEPROM.read(newAppAddress + i); //big-endian
      Serial.print(appEui[i]); Serial.print("  ");
    }
    Serial.println("");
    Serial.println("New AppEUI loaded.");

    Serial.print("New AppKey found.");
    for (int i = 0; i < 16; ++i)
    {
      appKey[i] = (uint8_t) EEPROM.read(newAppKeyAddress + i); //big-endian
      Serial.print(appKey[i]); Serial.print("  ");
    }
    Serial.println("");
    Serial.println("New AppKey loaded.");

    Serial.println("New DevEUI found.");
    for (int i = 0; i < 8; ++i)
    {
      devEui[i] = (uint8_t) EEPROM.read(newDevEUIAddress + i); //big-endian
      Serial.print(devEui[i]); Serial.print("  ");
    }
    Serial.println("");
    Serial.println("New DevEUI loaded.");
  } else {
    Serial.println("Key is zeroes and not valid. Using the original AppEUI");
    // key is zeroes and not valid
    useOrigApp = 'y';
    EEPROM.write(selectionAddress, byte(useOrigApp));
  }
}

void InitStoreKeys(void) {
  Serial.println("First time setup, Storing the original AppEUI...");
  // set useOrigApp to 'y'
  useOrigApp = 'y';
  EEPROM.write(selectionAddress, byte('y'));
  EEPROM.write(init_Address, byte('y'));
  // Save the default APPEUI in the Flash Memory
  for (int i = 0; i < 8; ++i)
  {
    EEPROM.write(defaultAppEUIAddress + i, byte(appEui[i])); //big-endian
  }
  for (int i = 0; i < 8; ++i)
  {
    EEPROM.write(defaultDevEUIAddress + i, byte(devEui[i])); //big-endian
  }
  for (int i = 0; i < 16; ++i)
  {
    EEPROM.write(defaultAppKeyAddress + i, byte(appKey[i])); //big-endian
  }
  Serial.println("Default Keys saved in the Flash Memory.");
}

TimerEvent_t joinTimeOut; // TTN join timeout counter

void startJoiningTTN(void) {
  // start the timer
  Serial.println("Join TimeOut TimerEvent_t started.");
  TimerInit(&joinTimeOut, joinTimedOutEvent);
  TimerSetValue(&joinTimeOut, 30000); // 30 seconds
  TimerStart(&joinTimeOut);
  LoRaWAN.join();
}

void joinFailureDebug(void) {
  Serial.println("Running Join Failure debug");
  EEPROM.begin(512);
  EEPROM.write(selectionAddress, byte('y'));
  Serial.println("Join Failed, loading defualt keys..");
  for (int i = 0; i < 8; i++) {
    EEPROM.write(newAppAddress, byte(0));
  }
  EEPROM.end();
}

void joinTimedOutEvent(void) {
  joinFailureDebug();
  //  reset the MCU
  Serial.println("Join TimedOut, resetting the MCU will join default TTN Application..");
  innerWdtEnable(false);
  delay(5000); //Wait until the MCU resets
}

void ifJoinedTTN(void) {
  //stop the Timer if it is running
  if (joinTimeOut.IsRunning) {
    TimerStop(&joinTimeOut);
    Serial.println("joinTimeOut Timer is stopped");
  }
}

void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println(F("Checking keys..."));
  EEPROM.begin(512);
  // Check init_ for 'y' or 'n'
  char init_ = char(EEPROM.read(init_Address));
  useOrigApp = char(EEPROM.read(selectionAddress));
  if ( init_ == 'y') {
    // already initialized and has a new key
    if (useOrigApp == 'n' || useOrigApp == 'N') {
      // Load new AppEUI
      Serial.println("Already initialized and have new keys.");
      LoadNewKeys();
    } else {
      Serial.println("Using default Keys...");
      // check if any of them are zeroes and load from Flash if needed
      for (int i = 0; i < 8; ++i)
      {
        devEui[i] = (uint8_t) EEPROM.read(defaultDevEUIAddress + i); //big-endian
      }

      for (int i = 0; i < 8; ++i)
      {
        appEui[i] = (uint8_t) EEPROM.read(defaultAppEUIAddress + i); //big-endian
      }

      for (int i = 0; i < 16; ++i)
      {
        appKey[i] = (uint8_t) EEPROM.read(defaultAppKeyAddress + i); //big-endian
      }

    }
  } else {
    // First time setup, store default keys
    InitStoreKeys();
  }
  EEPROM.end();
#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin(); // downlinks fail if this is not called!
}

void loop()
{
  switch ( deviceState )
  {
    case DEVICE_STATE_INIT:
      {
        Serial.println("Device state: DEVICE_STATE_INIT");
#if(LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
#if(AT_SUPPORT)
        Serial.println("Getting device parameters...");
        getDevParam();
#endif
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        Serial.println("Device state: DEVICE_STATE_JOIN");
        startJoiningTTN(); // Contains LoRaWAN.join() with joinTimeOut Event
        break;
      }
    case DEVICE_STATE_SEND:
      {
        Serial.println("Device state: DEVICE_STATE_SEND");
        ifJoinedTTN(); // Runs only once on the first packet TX
        prepareTxFrame( appPort );
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        Serial.println("Device state: DEVICE_STATE_CYCLE");
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        //Serial.println("Device state: DEVICE_STATE_SLEEP");
        LoRaWAN.sleep();
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}
