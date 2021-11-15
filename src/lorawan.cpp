#include "lorawan.h"
#include "maxbotix.h"
#include "sensorcfg.h"
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


bool autoFeed = false;      // Watchdog timer 
bool SEND_CFG_AS_UPLINK = true;        // Set to true at start and when there is a change in sensor cfg; used to send sensor cfg via uplink
bool CFG_CHANGE_DETECTED = false;    // detect if there is a sensor state change

/* EEPROM params */
const int selectionAddress = 8; // Single char (y/n) in ASCII
const int init_Address = 9;
const int newAppAddress = 12;    // 12 to 19
const int newAppKeyAddress = 21;  //21 to 37
const int newDevEUIAddress = 40;
const int defaultAppEUIAddress = 50; // 50 to 57
const int defaultAppKeyAddress = 58; // 58 to 73
const int defaultDevEUIAddress = 75; // 75 to 82
/*
  Semantic Versioning 2.0.0 Format:
        MAJOR_VERSION.MINOR_VERSION.PATCH_VERSION
*/
const int MAJOR_VERSION = 4; // incompatible changes
const int MINOR_VERSION = 0; // add functionality in a backwards compatible manner
const int PATH_VERSION = 0; // backwards compatible bug fixes

uint8_t SENSOR_STATE = 0x78;  // Default sensor state is 'stop' -> Uplinks are CFG packets

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
uint32_t appTxDutyCycle = 10000;
uint16_t TX_INTERVAL = 60;

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

void ModifyDutyCycle(McpsIndication_t *mcpsIndication){
        unsigned long dutycycle = 0;
        for (int i = 1; i < mcpsIndication->BufferSize; i++) {
                dutycycle =  (mcpsIndication->Buffer[i]) | ( dutycycle << 8*(i-1));
        }
        if (dutycycle!= 0) {
                Serial.print("Current duty cycle is: ");
                Serial.println(appTxDutyCycle);
                // Changing Duty Cycle
                TX_INTERVAL = dutycycle;
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
        case 2:
                ModifyDutyCycle(mcpsIndication);
                break;
        case 3:
                ModifyDutyCycle(mcpsIndication);
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

void ModifyKeys(McpsIndication_t *mcpsIndication) {
        EEPROM.begin(512);
        for (int i = 0; i < 8; ++i)
        {
                EEPROM.write(newAppAddress + i, byte(mcpsIndication->Buffer[i + 1])); //big-endian
        }
        for (int i = 0; i < 8; ++i)
        {
                EEPROM.write(newDevEUIAddress + i, byte(mcpsIndication->Buffer[i + 9])); //big-endian
        }
        for (int i = 0; i < 16; ++i)
        {
                EEPROM.write(newAppKeyAddress + i, byte(mcpsIndication->Buffer[i + 17])); //big-endian
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
                        Serial.print(appEui[i], HEX); Serial.print("  ");
                }
                Serial.println("");
                Serial.println("New AppEUI loaded.");

                Serial.print("New AppKey found.");
                for (int i = 0; i < 16; ++i)
                {
                        appKey[i] = (uint8_t) EEPROM.read(newAppKeyAddress + i); //big-endian
                        Serial.print(appKey[i], HEX); Serial.print("  ");
                }
                Serial.println("");
                Serial.println("New AppKey loaded.");

                Serial.println("New DevEUI found.");
                for (int i = 0; i < 8; ++i)
                {
                        devEui[i] = (uint8_t) EEPROM.read(newDevEUIAddress + i); //big-endian
                        Serial.print(devEui[i], HEX); Serial.print("  ");
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

uint8_t get_current_sensor_state(void){
        return SENSOR_STATE;
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

void process_operation(McpsIndication_t *mcpsIndication){
        if ((char(mcpsIndication->Buffer[1]) == 's') && (char(mcpsIndication->Buffer[2]) == 't') && (char(mcpsIndication->Buffer[3]) == 'a') && (char(mcpsIndication->Buffer[4]) == 'r') && (char(mcpsIndication->Buffer[5]) == 't')) {
                Serial.println("Start sensing, uplinks are Ultrasonic measurements.");
                SEND_CFG_AS_UPLINK = false;
                SENSOR_STATE = 0x73;
        } else if ((char(mcpsIndication->Buffer[1]) == 's') && (char(mcpsIndication->Buffer[2]) == 't') && (char(mcpsIndication->Buffer[3]) == 'o') && (char(mcpsIndication->Buffer[4]) == 'p')) {
                Serial.println("Stop sensing, uplinks are sensor CFG packets.");
                SEND_CFG_AS_UPLINK = true;
                SENSOR_STATE = 0x78;
        } else if ((char(mcpsIndication->Buffer[1]) == 'r') && (char(mcpsIndication->Buffer[2]) == 'e') && (char(mcpsIndication->Buffer[3]) == 's') && (char(mcpsIndication->Buffer[4]) == 'e') && (char(mcpsIndication->Buffer[5]) == 't')) {
                Serial.println("Reset Sensor.");
                SENSOR_STATE = 0x72;
                // resets after updating it's CFG
        } else {
                Serial.println("Invalid Operation");
        }
}

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

        switch (mcpsIndication->Buffer[0]) {
        case 0x4D:
                /* Sensor Mode Change
                   Packet format:
                  |   oper     | Duty Cycle in seconds  | Sensor Mode | Sampling Rate  |  Number of readings per measurement  |
                  |   0x4D     |       2 bytes          |    1 byte   |    2 bytes     |        1 byte                        |
                 */
                CFG_CHANGE_DETECTED = true;
                ModifySensorSettings(mcpsIndication);
                Serial.println("Modify Sensor Settings");
                break;
        case 0x41:
                /* Sensor APP Change
                   Packet format:
                  |   oper     |   AppEUI(big-endian)   |   DevEUI(big-endian)  |   AppKEY(big-endian)    |
                  |   0x41     |         8 bytes        |          8 bytes      |         16 bytes        |
                 */
                Serial.println("Change APPEUI");
                if (mcpsIndication->BufferSize == 33) // 33: 0x41, AppEUI, AppKey, DevEUI
                {
                        CFG_CHANGE_DETECTED = true;
                        // Modify AppEUI
                        ModifyKeys(mcpsIndication);
                        // Reset
                        innerWdtEnable(false);
                        delay(5000); //Wait until the MCU resets
                } else {
                        Serial.println("Invalid Keys!");
                }
                break;
        case 0x4F:
                /*
                  Sensor Operation control: Start/ Stop/ Reset
                      Start: measurement uplinks
                      Stop: cfg packet uplinks
                      Reset: reset using watchdog timer
                */
                CFG_CHANGE_DETECTED = true;
                Serial.print("Sensor Operation Control  --->  ");
                process_operation(mcpsIndication);
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
        Serial.println("Preparing TX frame...");
        byte lowbyte, highbyte, lowbat, highbat;

        // Error flags
        ERROR_FLAGS = 0x00;


        /* If sensor state is changed, update the server (OR) If uplinks are CFGs, update the server with current cfg*/
        if (CFG_CHANGE_DETECTED == true || SEND_CFG_AS_UPLINK == true){
                /*
                   CFG update uplink Format:
                 | Error Flag  |   sensor_sleep   |    sensor_agg     |   sensor_meas_delta     | sensor_reading_count   |    sensor_state   |    fw_ver       |
                 |    255 (FF) |     2 bytes      |      1 byte       |          2 bytes        |          1 byte        |        1 byte     |    6 bytes      |
                 
                  Sensor State:
                  |  Start  |   Stop  |  Reset  |
                  |   's'   |   'x'   |   'r'   |

                 */
                appDataSize = 11;
                ERROR_FLAGS = 255;
                appData[0] = (unsigned char)ERROR_FLAGS;
                uint16_t tx_interval_bytes = TX_INTERVAL;
                
                // sensor_sleep
                byte lowduty = lowByte(tx_interval_bytes);
                byte highduty = highByte(tx_interval_bytes);
                appData[1] = (unsigned char)lowduty;
                appData[2] = (unsigned char)highduty;

                // sensor_agg
                appData[3] = (unsigned char)sensorMode;

                // sensor_meas_delta
                lowbyte = lowByte(sensor_sampling_rate);
                highbyte = highByte(sensor_sampling_rate);
                appData[4] = (unsigned char)lowbyte;
                appData[5] = (unsigned char)highbyte;

                // sensor_reading_count
                appData[6] = (unsigned char)sensor_numberOfReadings;

                // sensor_state
                appData[7] = (unsigned char)get_current_sensor_state();

                // firmware version
                // Major
                appData[8] = (unsigned char)MAJOR_VERSION;
                // Minor 
                appData[9] = (unsigned char)MINOR_VERSION;
                // Patch
                appData[10] = (unsigned char)PATH_VERSION;
                
                // Update only once
                if (CFG_CHANGE_DETECTED == true){
                        CFG_CHANGE_DETECTED = false;
                }

        } else {

                // Regular Uplink contains: Sensor Error Flags followed by Battery and then Sensor Data

                /* 
                 |-------LoraWAN uplink packet format-----------------|
                 | Error flags  | Battery Level | Ultrasonic reading  |
                 |   1 byte     |    2 bytes    |        2 bytes      |

                 |-----Ultrasonic reading------|
                 |           2 bytes           |
                 |    high byte | low byte     |
                 
                 |-------Battery Level-------|
                 |           2 bytes         |
                 |    high byte | low byte   |
                 
                 |----------------------------------------------Error Flags  ------------------------------------------------------|
                 |     bit 7                       |  bit 6   |  bit 5  |  bit 4  |  bit 3  |  bit 2  |  bit 1  |      bit 0       |
                 |     Used only for CFG update    |          |         |         |         |         |         |   SD error flag  |
                 */

                // Regular Uplink Packet size
                appDataSize = 5;

                // Maxbotix
                distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
                Serial.print("Distance = ");
                Serial.print(distance);
                Serial.println(" mm");

                // Battery
                batLevel = getBatteryVoltage(); /*  get the BatteryVoltage in mV. */
                Serial.print("Battery Level = ");
                Serial.print(batLevel);
                Serial.println(" V");

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

}

TimerEvent_t joinTimeOut; // TTN join timeout counter

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

void startJoiningTTN(void) {
        // start the timer
        Serial.println("Join TimeOut TimerEvent_t started.");
        TimerInit(&joinTimeOut, joinTimedOutEvent);
        TimerSetValue(&joinTimeOut, 30000); // 30 seconds
        TimerStart(&joinTimeOut);
        LoRaWAN.join();
}


void setup_lorawan(unsigned int packet_interval) {
        Serial.println(F("Setting up dutycycle..."));
        TX_INTERVAL = packet_interval;
        Serial.print(F("Dutycycle set to "));Serial.print(TX_INTERVAL);Serial.println(F(" seconds"));
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
#if (AT_SUPPORT)
        enableAt();
#endif
        deviceState = DEVICE_STATE_INIT;
        LoRaWAN.ifskipjoin(); // downlinks fail if this is not called!
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
                Serial.println("Getting device parameters...");
                getDevParam();
#endif
                printDevParam();
                LoRaWAN.init(loraWanClass, loraWanRegion);
                SEND_CFG_AS_UPLINK = true;
                deviceState = DEVICE_STATE_JOIN;
                break;
        }
        case DEVICE_STATE_JOIN:
        {
                startJoiningTTN(); // Contains LoRaWAN.join() with joinTimeOut Event
                break;
        }
        case DEVICE_STATE_SEND:
        {
                ifJoinedTTN(); // Runs only once on the first packet TX
                prepareTxFrame(appPort);
                // if CFG, sleep less time
                if (SEND_CFG_AS_UPLINK){
                        appTxDutyCycle = 10 * 1000; 
                } else {
                        appTxDutyCycle = TX_INTERVAL * 1000;
                }
                LoRaWAN.send();
                deviceState = DEVICE_STATE_CYCLE;
                break;
        }
        case DEVICE_STATE_CYCLE:
        {
                // Schedule next packet transmission
                txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
                LoRaWAN.cycle(txDutyCycleTime);
                // check if state is reset
                if (SENSOR_STATE == 0x72){
                        // Reset
                        innerWdtEnable(false);
                        delay(5000); //Wait until the MCU resets       
                }
                Serial.print("Going to sleep, next uplink in "); Serial.print(appTxDutyCycle/1000);Serial.println(" s.");
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
