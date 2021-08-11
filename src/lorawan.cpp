#include "lorawan.h"
#include "maxbotix.h"
#include <EEPROM.h>
#include "sensorcfg.h"
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
// start reading from the first byte (address 0) of the EEPROM
const int defaultAppAddress = 0; // 0 to 7
const int selectionAddress = 8; // Single char (y/n) in ASCII
const int init_Address = 9;
const int newAppAddress = 12;    // 12 to 19
const int newAppKeyAddress = 21;  //21 to 37

unsigned long joinFailCounter = 0;          // counter for Join failure
unsigned char cfg_packet[7];      // CFG Uplink Packet
unsigned char lora_packet[5];     // Regular Uplink Packet
bool UPDATE_CONFIG = true;        // Set to true at start and when there is a change in sensor cfg; used to send sensor cfg via uplink
unsigned int ERROR_FLAGS;
char useOrigApp = 'y';
char init_ = 'n'; //Set to non zero value after saving default keys on the first boot

/* OTAA para*/
uint8_t devEui[] = TTN_DEVEUI;
uint8_t appEui[] = TTN_APPEUI;
uint8_t appKey[] = TTN_APPKEY;

/* ABP para*/
uint8_t nwkSKey[] = { };
uint8_t appSKey[] = { };
uint32_t devAddr =  ( uint32_t )0x0000;

/*LoraWan channelsmask, default channels 0-7*/
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }; // 0xFF00 from 00FF

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t loraWanClass = LORAWAN_CLASS;

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
#define timetillwakeup 10000
static TimerEvent_t sleep;
static TimerEvent_t wakeUp;
uint8_t lowpower = 1;

void JoinFailCounter(void) {
  if((millis()/1000)%2 == 0){
    joinFailCounter = millis();
  }
}

bool JoinFailed(void) {
        if (millis() - joinFailCounter > 15000) {
                return true;
        } else {
                return false;
        }
}

void JoinFailureDebug(void) {
        EEPROM.begin(512);
        if(JoinFailed() && useOrigApp == 'n') {

                // Use old AppEUI
                useOrigApp = 'y';
                EEPROM.write(selectionAddress, (byte)(useOrigApp));

                // discard failed AppEUI
                for (int i = 0; i < 8; ++i)
                {
                        EEPROM.write(newAppAddress + i, 0);
                }

                // restart
                innerWdtEnable(autoFeed);
        } else {
                Serial.println("Join falied. Check the default Keys loaded while flashing the MCU!");
        }
        EEPROM.commit();
        EEPROM.end();
}

void onSleep()
{
        //Serial.printf("Going into lowpower mode, %d ms later wake up.\r\n", timetillwakeup);
        lowpower = 1;
        //  Radio.Sleep( );
        //timetillwakeup ms later wake up;
        TimerSetValue( &wakeUp, timetillwakeup );
        TimerStart( &wakeUp );
}

void onWakeUp()
{
        //Serial.printf("\r\nWoke up, %d ms later.\r\n", timetillwakeup);
        lowpower = 0;
        //  //timetillsleep ms later into lowpower mode;
        //  TimerSetValue( &sleep, timetillsleep );
        //  TimerStart( &sleep );
}

void ModifyDutyCycle(McpsIndication_t *mcpsIndication){
        unsigned long dutycycle = 0;
        for (int i = 1; i < 3; i++) {
                dutycycle =  (mcpsIndication->Buffer[i]) | ( dutycycle << 8*i);
        }
        if (dutycycle!= 0) {
                Serial.print("Current duty cycle is: ");
                Serial.println(appTxDutyCycle);
                // Changing Duty Cycle
                appTxDutyCycle = dutycycle;
                Serial.print("Updated dutycycle is: ");
                Serial.println(appTxDutyCycle);
        } else{
                Serial.println("Dutycycle is the same.");
        }
}

void ModifySensorMode(McpsIndication_t *mcpsIndication){
        unsigned int sensorMode_ = 0;
        sensorMode_ =  (mcpsIndication->Buffer[3]) | ( sensorMode_ );
        if (sensorMode_ > 0 && sensorMode_ <= 3 ) {
                Serial.print("Current sensorMode is: ");
                Serial.println(sensorMode);
                // Changing Sensor Mode
                sensorMode = sensorMode_;
                Serial.print("Updated sensorMode is: ");
                Serial.println(sensorMode);
        } else{
                Serial.println("Sensor Mode is the same.");
        }
}

void ModifySamplingRate(McpsIndication_t *mcpsIndication){
        unsigned int sampling_rate = 0;
        sampling_rate =  (mcpsIndication->Buffer[4]) | ( sampling_rate );
        sampling_rate =  (mcpsIndication->Buffer[5]) | ( sampling_rate << 8);
        if (sampling_rate!= 0 ) {
                Serial.print("Current sensor sampling rate is: ");
                Serial.println(sensor_sampling_rate);
                // Changing Sensor Mode
                sensor_sampling_rate = sampling_rate;
                Serial.print("Updated sensor sampling rate is: ");
                Serial.println(sensor_sampling_rate);
        } else{
                Serial.println("Sensor sampling rate is the same.");
        }
}

void ModifyNumberOfSamples(McpsIndication_t *mcpsIndication){
        unsigned int numb_readings = 0;
        numb_readings =  (mcpsIndication->Buffer[6]) | ( numb_readings );
        if (numb_readings!= 0 && numb_readings<20) {
                Serial.print("Current number of readings per measurement: ");
                Serial.println(sensor_numberOfReadings);
                // Changing Sensor Mode
                sensor_numberOfReadings = numb_readings;
                Serial.print("Updated number of readings per measurement: ");
                Serial.println(sensor_numberOfReadings);
        } else{
                Serial.println("Sensor number of readings per measurement is the same.");
        }
}

void ModifySensorSettings(McpsIndication_t *mcpsIndication){

        switch(mcpsIndication->BufferSize) {
        case 3:
                ModifyDutyCycle(mcpsIndication);
                ModifySensorMode(mcpsIndication);
                break;
        case 4:
                ModifyDutyCycle(mcpsIndication);
                ModifySensorMode(mcpsIndication);
                break;
        case 6:
                ModifyDutyCycle(mcpsIndication);
                ModifySensorMode(mcpsIndication);
                ModifySamplingRate(mcpsIndication);
                break;
        case 7:
                ModifyDutyCycle(mcpsIndication);
                ModifySensorMode(mcpsIndication);
                ModifySamplingRate(mcpsIndication);
                ModifyNumberOfSamples(mcpsIndication);
                break;
        default:
                Serial.println("Invalid Sensor settings received.");
                break;
        }
}

void ModifyAppEUI(McpsIndication_t *mcpsIndication){
        EEPROM.begin(512);
        for (int i = 0; i < 8; ++i)
        {
                EEPROM.write(newAppAddress + i, byte(mcpsIndication->Buffer[i+1])); //big-endian
        }
        for (int i = 0; i < 16; ++i)
        {
                EEPROM.write(newAppKeyAddress + i, byte(mcpsIndication->Buffer[i+9])); //big-endian
        }
        useOrigApp = 'n'; // Use new AppEUI on restart
        EEPROM.write(selectionAddress, byte(useOrigApp)); // Save the choice to Flash Memory
        // restart
        EEPROM.commit();
        EEPROM.end();
        delay(500);
        Serial.println("Keys loaded...Resetting the MCU.");
        innerWdtEnable(autoFeed);
}

//downlink data handle function example
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
        Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
        Serial.print("+REV DATA:");
        for(uint8_t i=0; i<mcpsIndication->BufferSize; i++)
        {
                Serial.printf("%02X",mcpsIndication->Buffer[i]);
        }
        Serial.println();
        uint32_t color=mcpsIndication->Buffer[0]<<16|mcpsIndication->Buffer[1]<<8|mcpsIndication->Buffer[2];
#if (LoraWan_RGB==1)
        turnOnRGB(color,5000);
        turnOffRGB();
#endif
        // process received downlink
        /*
         * Downlink Packet format:
         *    |   oper     | Duty Cycle in seconds  | Sensor Mode | Sampling Rate  |  Number of readings per measurement  |
         *    |  1 byte    |       2 bytes          |    1 byte   |    2 bytes     |        1 byte                        |
         */
        // set UPDATE_CONFIG to true
        UPDATE_CONFIG = true;
        switch(mcpsIndication->Buffer[0]) {
        case 0x4D:
                /* Change sensor Mode*/
                ModifySensorSettings(mcpsIndication);
                UPDATE_CONFIG == true;
                break;
        case 0x41:
                /* Change APPEUI*/
                Serial.println("Change APPEUI");
                if (mcpsIndication->BufferSize == 25)
                {
                        ModifyAppEUI(mcpsIndication);
                } else {
                        Serial.println("Invalid AppEUI and AppKey size!");
                }
                break;
        default:
                Serial.println("Invalid downlink format!");
                break;
        }
}

uint16_t distance;
uint16_t batLevel;

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

        byte lowbyte, highbyte, lowbat, highbat;
        // Error
        ERROR_FLAGS = 0x00;

        // Formatting payload
        if (UPDATE_CONFIG == true) {
                // Send Sensor Config via Uplink
                /*
                   CFG update uplink Format:
                 | Error Flag  | Sensor Mode | Sensor Sampling Rate | Sensor Number of Readings |
                 |    255 (FF) |    1 byte   |      2 bytes         |        1 bytes            |
                 */
                appDataSize = 7;
                ERROR_FLAGS = 255;
                appData[0] = (unsigned char)ERROR_FLAGS;
                uint16_t TX_INTERVAL = appTxDutyCycle/1000;
                byte lowduty = lowByte(TX_INTERVAL);
                byte highduty = highByte(TX_INTERVAL);
                appData[1] = (unsigned char)lowduty;
                appData[2] = (unsigned char)highduty;
                appData[3] = (unsigned char)sensorMode;
                lowbyte = lowByte(sensor_sampling_rate);
                highbyte = highByte(sensor_sampling_rate);
                appData[4] = (unsigned char)lowbyte;
                appData[5] = (unsigned char)highbyte;
                appData[6] = (unsigned char)sensor_numberOfReadings;
        } else {
                // Regular Uplink contains: Sensor Error Flags followed by Battery and then Sensor Data

                /* LoraWAN uplink packet format
                 | Error flags  | Battery Level | Ultrasonic reading  |
                 |   1 byte     |    2 bytes    |        2 bytes      |
                 |     Ultrasonic reading      |
                 |           2 bytes           |
                 |    high byte | low byte     |
                 |       Battery Level       |
                 |           2 bytes         |
                 |    high byte | low byte   |
                 |------------------------------------------------------------ Error Flags  ----------------------------------------------------------------|
                 |     bit 7                                                |  bit 6   |  bit 5  |  bit 4  |  bit 3  |  bit 2  |  bit 1  |      bit 0       |
                 |     Used only for CFG update (all other bits are high)   |          |         |         |         |         |         |   SD error flag  |
                 */

                // Regular Uplink Packet size
                appDataSize = 5;

                // Maxbotix
                distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
                Serial.print("Distance = ");
                Serial.print(distance);
                Serial.println(" mm");

                // Battery
                batLevel = getBatteryVoltage();

                // Payload
                lowbat = lowByte(batLevel);
                highbat = highByte(batLevel);
                appData[0] = (unsigned char)ERROR_FLAGS;
                appData[1] = (unsigned char)lowbat; //we're unsigned
                appData[2] = (unsigned char)highbat;
                lowbyte = lowByte(distance);
                highbyte = highByte(distance);
                appData[3] = (unsigned char)lowbyte;
                appData[4] = (unsigned char)highbyte;
        }
        UPDATE_CONFIG = false;
        onSleep();
}

void InitStoreAPPEUI(void){
        EEPROM.begin(512);
        Serial.println("First time setup, Storing the original AppEUI...");
        // set useOrigApp to 'y'
        useOrigApp = 'y';
        EEPROM.write(selectionAddress, byte(useOrigApp));
        EEPROM.write(init_Address, byte('y'));
        // Save the default APPEUI in the Flash Memory
        for (int i = 0; i < 8; ++i)
        {
                EEPROM.write(defaultAppAddress + i, byte(appEui[i])); //big-endian
        }
        Serial.println("Default AppEUI saved in the Flash Memory.");
        EEPROM.commit();
        EEPROM.end();
}

void LoadNewAppEUI(void){
        EEPROM.begin(512);
        bool foundNewAppEUI;
        for (int i = 0; i < 8; ++i)
        {
                if(int(EEPROM.read(newAppAddress))!= 0) {
                        // break out of the loop if atleast one non-zero byte is found
                        foundNewAppEUI = true;
                        break;
                } else {
                        // all zeroes is default key
                        foundNewAppEUI = false;
                }
        }
        // for (int i = 0; i < 16; ++i)
        // {
        //         if(int(EEPROM.read(newAppKeyAddress))!= 0) {
        //                 // break out of the loop if atleast one non-zero byte is found
        //                 foundNewAppEUI = true;
        //                 break;
        //         } else {
        //                 // all zeroes is default key
        //                 foundNewAppEUI = false;
        //         }
        // }
        if (foundNewAppEUI) {
                // load new keys
                Serial.println("New AppEUI found");
                for (int i = 0; i < 8; ++i)
                {
                        appEui[i] = byte(EEPROM.read(newAppAddress + i)); //big-endian
                }
                Serial.println("New AppEUI loaded.");
                for (int i = 0; i < 16; ++i)
                {
                        appKey[i] = byte(EEPROM.read(newAppKeyAddress + i)); //big-endian
                }
                Serial.println("New AppKey loaded.");
        } else {
                Serial.println("Key is zeroes and not valid. Using the original AppEUI");
                // key is zeroes and not valid
                useOrigApp = 'y';
                EEPROM.write(selectionAddress, byte(useOrigApp));
        }
        EEPROM.commit();
        EEPROM.end();
}

void setup_lorawan(unsigned int packet_interval) {
        Serial.begin(115200);
        Serial.println(F("Setting up EEPROM..."));
        Serial.println(F("Checking keys..."));
        EEPROM.begin(512);
        // Check init_ for 'y' or 'n'
        if ( char(EEPROM.read(init_Address)) == 'y') {
          useOrigApp = char(EEPROM.read(selectionAddress));
          if (useOrigApp == 'n' || useOrigApp == 'N') {
                  // Load new AppEUI
                  LoadNewAppEUI();
          }
        } else {
          // First time setup...
          InitStoreAPPEUI();
        }
        delay(1000);

        Serial.println(F("Setting up LoraWAN..."));
        // Set DutyCycle
        appTxDutyCycle = packet_interval*1000;
        boardInitMcu();
        pinMode(Vext, OUTPUT);
        digitalWrite(Vext, HIGH);

#if (AT_SUPPORT)
        enableAt();
#endif

        deviceState = DEVICE_STATE_INIT;
        LoRaWAN.ifskipjoin();
        Serial.println("Setup done!");
        TimerInit( &sleep, onSleep );
        TimerInit( &wakeUp, onWakeUp );
        delay(500);
        onSleep();
        EEPROM.commit();
        EEPROM.end();
}

void lorawan_runloop_once(void)
{
        switch ( deviceState )
        {
        case DEVICE_STATE_INIT:
        {
#if (LORAWAN_DEVEUI_AUTO)
                LoRaWAN.generateDeveuiByChipID();
#endif
#if (AT_SUPPORT)
                getDevParam();
#endif
                printDevParam();
                LoRaWAN.init(loraWanClass, loraWanRegion);
                deviceState = DEVICE_STATE_JOIN;
                break;
        }
        case DEVICE_STATE_JOIN:
        {
                LoRaWAN.join();
                JoinFailCounter();
                if(JoinFailed()) {
                        JoinFailureDebug();
                }
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
