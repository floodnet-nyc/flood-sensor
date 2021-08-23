#include "LoRaWan_APP.h"
#include "Arduino.h"
#include <EEPROM.h>
#ifdef __asr650x__
#include "innerWdt.h"
#endif


#define triggerPin GPIO8
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
#define TTN_APPEUI { }

// This should also be in big endian format
#define TTN_DEVEUI { }

// This key should be in big endian format
#define TTN_APPKEY { }

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
uint32_t appTxDutyCycle = 15000;

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


unsigned long maxTimeOut;

// Maxbotix Ultrasonic variables
uint16_t readings_arr[30] = {0};
size_t n = *(&readings_arr + 1) - readings_arr;

unsigned int sensorMode;
unsigned int sensor_sampling_rate;
unsigned int sensor_numberOfReadings;

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

void sort(uint16_t a[], size_t n) {
  int i, j, temp;

  for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - i - 1; j++) {
      if (a[j] < a[j + 1])
        swap(&a[j], &a[j + 1]);
    }
  }
}

uint16_t mean(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings) {
  uint16_t readings_sum = 0;
  for (int i = 0; i < sensor_numberOfReadings; i++) {
    readings_sum = readings_sum + readings_arr[i];
  }
  return readings_sum / sensor_numberOfReadings;
}

uint16_t median(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings) {
  if (sensor_numberOfReadings % 2 != 0) {
    return readings_arr[(sensor_numberOfReadings + 1) / 2 - 1];
  }
  else {
    return (readings_arr[sensor_numberOfReadings / 2 - 1] + readings_arr[sensor_numberOfReadings / 2]) / 2;
  }
}

uint16_t mode(uint16_t readings_arr[], size_t n, unsigned int sensor_numberOfReadings) {
  int counter = 1;
  int max = 0;
  uint16_t mode_ = readings_arr[0];
  for (int i = 0; i < sensor_numberOfReadings - 1; i++) {
    if ( readings_arr[i] == readings_arr[i + 1] ) {
      counter++;
      if ( counter > max ) {
        max = counter;
        mode_ = readings_arr[i];
      }
    } else
      counter = 1;          // reset counter.
  }
  return mode_;
}

void setup_maxbotix(unsigned int mode = 2, unsigned int sampling_rate = 250 , unsigned int numberOfReadings = 7) {
  Serial.println("Setting up Maxbotix .... ");
  digitalWrite(Vext, HIGH);   //power line: now off
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);  //trigger line: now off
  Serial1.begin(9600);
  Serial.println("Sensor Settings:");
  sensorMode = mode;
  Serial.print("    Sensor mode: ");
  Serial.println(sensorMode);
  sensor_sampling_rate = sampling_rate;
  Serial.print("    Sensor sampling rate: ");
  Serial.println(sensor_sampling_rate);
  sensor_numberOfReadings = numberOfReadings;
  Serial.print("    Number of readings per measurement: ");
  Serial.println(sensor_numberOfReadings);
}

uint16_t sensor_singleread(void) {
  int distance = 0;
  char serialbuffer[4];
  int index = 0;
  char rc;
  Serial1.flush();
  delay(150);
  boolean newData = false;
  while (newData == false) {
    if (Serial1.available())
    {
      char rc = Serial1.read();
      if (rc == 'R')
      {
        while (index < 3)
        {
          if (Serial1.available())
          {
            serialbuffer[index] = Serial1.read();
            index++;
          }
        }
      }
      newData = true;
    }
  }
  if (newData) {
    distance = (serialbuffer[0] - '0') * 1000 + (serialbuffer[1] - '0') * 100 + (serialbuffer[2] - '0') * 10 + (serialbuffer[3] - '0');
    Serial.print("distance: "); Serial.print(distance); Serial.println(" mm");
  }
  return distance;
}


uint16_t read_sensor_using_modes(unsigned int sensorMode, unsigned int sensor_sampling_rate = 20, unsigned int sensor_numberOfReadings = 5)
{
  uint16_t distance = 0;
  Serial.println("Reading sensor using modes, entering measurements into an array...");
  Serial.println("VEXT Low: Sensor Powered up");
  digitalWrite(Vext, LOW);
  digitalWrite(triggerPin, HIGH);
  Serial.println("Trigger Pin High: Sensor reading...");
  sensor_singleread();
  delay(sensor_sampling_rate);
  for (int i = 0; i < sensor_numberOfReadings; i++) {
    readings_arr[i] = sensor_singleread();
    delay(sensor_sampling_rate);
  }
  digitalWrite(triggerPin, LOW);
  Serial.println("Trigger Pin Low: Sensor stopped.");
  Serial.println("VEXT High: Sensor turned off");
  digitalWrite(Vext, HIGH);

  Serial.println("Printing the array...");
  for (int i = 0; i < n; i++) {
    Serial.print(readings_arr[i]); Serial.print(" ");
  }
  Serial.println("");
  sort(readings_arr, n);
  Serial.println("Printing the sorted array...");
  for (int i = 0; i < n; i++) {
    Serial.print(readings_arr[i]); Serial.print(" ");
  }
  Serial.println("");

  switch (sensorMode) {
    case 1:
      // Mean
      distance = mean(readings_arr, n, sensor_numberOfReadings);
      break;
    case 2:
      // Median
      distance = median(readings_arr, n, sensor_numberOfReadings);
      break;
    case 3:
      // Mode
      distance = mode(readings_arr, n, sensor_numberOfReadings);
      break;
    default:
      // Single Pulse-In single reading
      distance = sensor_singleread();
      break;
  }

  Serial.println("Cleaning measurements array...");
  for (int i = 0; i < n; i++) {
    readings_arr[i] = 0;
  }
  Serial.println("");
  Serial.println("Printing the cleaned array...");
  for (int i = 0; i < n; i++) {
    Serial.print(readings_arr[i]); Serial.print(" ");
  }
  Serial.println("");
  return distance;

}

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
  // Regular Uplink Packet size
  byte lowbyte, highbyte, lowbat, highbat;

  // Regular Uplink Packet size
  appDataSize = 5;

  // Maxbotix
  Serial.println("VEXT Low: Sensor Powered up");
  digitalWrite(Vext, LOW);
  digitalWrite(triggerPin, HIGH);
  Serial.println("Trigger Pin High: Sensor reading...");
  uint16_t  distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
  digitalWrite(triggerPin, LOW);
  Serial.println("Trigger Pin Low: Sensor stopped.");
  Serial.println("VEXT High: Sensor turned off");
  digitalWrite(Vext, HIGH);

  // Battery
  uint16_t batLevel = getBatteryVoltage();

  // Payload
  lowbat = lowByte(batLevel);
  highbat = highByte(batLevel);
  appData[0] = (unsigned char)0;
  appData[1] = (unsigned char)lowbat; //we're unsigned
  appData[2] = (unsigned char)highbat;
  lowbyte = lowByte(distance);
  highbyte = highByte(distance);
  appData[3] = (unsigned char)lowbyte;
  appData[4] = (unsigned char)highbyte;
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
  setup_maxbotix(2, 250, 7);
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
