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

/* Watchdog timer */
bool autoFeed = false;

/* EEPROM params */
const int selectionAddress = 8; // Single char (y/n) in ASCII
const int init_Address = 9;
const int newAppAddress = 12;    // 12 to 19
const int newAppKeyAddress = 21;  //21 to 37
const int newDevEUIAddress = 40;
char useOrigApp = 'y';

/* OTAA para*/
uint8_t devEui[] = {  };
uint8_t appEui[] = {  };
uint8_t appKey[] = {  };

/* ABP para*/
uint8_t nwkSKey[] = {  };
uint8_t appSKey[] = {  };
uint32_t devAddr =  ( uint32_t )0x00000000;

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
        ModifyAppEUI(mcpsIndication);
        // Load keys
        EEPROM.begin(512);
        LoadNewAppEUI();
        EEPROM.end();
        Serial.println("Keys loaded...Start OTAA again...");
        deviceState = DEVICE_STATE_JOIN;
      } else {
        Serial.println("Invalid AppEUI and AppKey size!");
      }
      break;
    default:
      Serial.println("Invalid downlink format!");
      break;
  }
}

void ModifyAppEUI(McpsIndication_t *mcpsIndication) {
  EEPROM.begin(512);
  for (int i = 0; i < 8; ++i)
  {
    EEPROM.write(newAppAddress + i, byte(mcpsIndication->Buffer[i + 1])); //big-endian
  }
  for (int i = 0; i < 16; ++i)
  {
    EEPROM.write(newAppKeyAddress + i, byte(mcpsIndication->Buffer[i + 9])); //big-endian
  }
  useOrigApp = 'n'; // Use new AppEUI on restart
  EEPROM.write(selectionAddress, byte(useOrigApp)); // Save the choice to Flash Memory
  // restart
  EEPROM.end();
  delay(500);
}

void LoadNewAppEUI(void) {
  bool foundNewAppEUI;
  for (int i = 0; i < 8; ++i)
  {
    if (int(EEPROM.read(newAppAddress)) != 0) {
      // break out of the loop if atleast one non-zero byte is found
      foundNewAppEUI = true;
      break;
    } else {
      // all zeroes is default key
      foundNewAppEUI = false;
    }
  }
  if (foundNewAppEUI) {
    // load new keys
    Serial.print("New AppEUI found.");
    for (int i = 0; i < 8; ++i)
    {
      appEui[i] = (uint8_t) EEPROM.read(newAppAddress + i); //big-endian
    }
    Serial.println("");
    Serial.println("New AppEUI loaded.");
    
    Serial.print("New AppKey found.");
    for (int i = 0; i < 16; ++i)
    {
      appKey[i] = (uint8_t) EEPROM.read(newAppKeyAddress + i); //big-endian
    }
    Serial.println("");
    Serial.println("New AppKey loaded.");

    Serial.println("New DevEUI found.");
    for (int i = 0; i < 8; ++i)
    {
      devEui[i] = (uint8_t) EEPROM.read(newDevEUIAddress + i); //big-endian
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

void setup() {
  delay(3000);
  Serial.begin(115200);
#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();
}

void loop()
{
  switch ( deviceState )
  {
    case DEVICE_STATE_INIT:
      {
#if(LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
#if(AT_SUPPORT)
        getDevParam();
#endif
        Serial.println(F("Checking keys..."));
        EEPROM.begin(512);
        useOrigApp = char(EEPROM.read(selectionAddress));
        if (useOrigApp == 'n' || useOrigApp == 'N') {
          // Load new AppEUI
          LoadNewAppEUI();
        } else {
          Serial.println("Using default Keys...");
        }
        EEPROM.end();
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
        prepareTxFrame( appPort );
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
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
