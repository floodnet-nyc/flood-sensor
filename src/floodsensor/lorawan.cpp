#include "lorawan.h"
#include "maxbotix.h"
#include "sensorcfg.h"
#include <EEPROM.h>
#include "rg15.h"
#include "tipping_bucket.h"
#include "sensorcfg.h"
#include "ttncredentials.h"





/*Sensor states*/
#define SENSING_STATE 0x73
#define RESET_STATE 0x72
#define CFG_STATE 0x78
char is_deployed;
char is_started;


/* EEPROM params */
const int selectionAddress = 8; // Single char (y/n) in ASCII
const int init_Address = 9;
const int newAppAddress = 12;    // 12 to 19
const int newAppKeyAddress = 21;  //21 to 37
const int newDevEUIAddress = 40;
const int defaultAppEUIAddress = 50; // 50 to 57
const int defaultAppKeyAddress = 58; // 58 to 73
const int defaultDevEUIAddress = 75; // 75 to 82
const int isDeployed = 90;
const int isStarted = 91;
/*
  Semantic Versioning 2.0.0 Format:
        MAJOR_VERSION.MINOR_VERSION.PATCH_VERSION
*/
const int MAJOR_VERSION = 4; // incompatible changes
const int MINOR_VERSION = 2; // add functionality in a backwards compatible manner
const int PATCH_VERSION = 0; // backwards compatible bug fixes


/*flags*/
bool autoFeed = false;      // Watchdog timer
bool overTheAirActivation = 1;
bool loraWanAdr = false;
bool keepNet = false;
bool isTxConfirmed = 0;
unsigned int ERROR_FLAGS;


/*SYS params*/
uint8_t SENSOR_STATE;


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

/*LoraWan region, override IDE defaults*/
LoRaMacRegion_t loraWanRegion = LORAMAC_REGION_US915;

/*LoraWan Class A*/
DeviceClass_t loraWanClass = CLASS_A;

/*LoRaWAN application data transmission duty cycle.  value in [ms]. DEFAULT is 10 seconds*/
uint32_t appTxDutyCycle = 10 * 1000;

/*User defined variable - TX_INTERVAL, changes between states as needed. DEFAULT is 10 seconds*/
uint16_t TX_INTERVAL = 10;
uint8_t appPort = 2;
uint8_t confirmedNbTrials = 4;
uint8_t lowpower = 1;

/*Sensor specific variables*/
uint16_t counter_rg15 = 0;
uint16_t bucket_tip_count = 0;


/******************************************************************************************

  Functions

*******************************************************************************************/

void ModifyDutyCycle(McpsIndication_t *mcpsIndication) {
        unsigned long dutycycle = 0;
        for (int i = 1; i < mcpsIndication->BufferSize; i++) {
                dutycycle =  (mcpsIndication->Buffer[i]) | ( dutycycle << 8 * (i - 1));
        }
        if (dutycycle != 0 && dutycycle < 1000) {
                Serial.print("Current duty cycle is: ");
                Serial.println(TX_INTERVAL);
                // Changing Duty Cycle
                TX_INTERVAL = dutycycle;
                Serial.print("Updated dutycycle is: ");
                Serial.println(TX_INTERVAL);
        } else {
                Serial.println("Dutycycle is the same.");
        }
}

void ModifySensorMode(McpsIndication_t *mcpsIndication) {
        unsigned int sensorMode_ = 0;
        sensorMode_ =  (mcpsIndication->Buffer[3]) | ( sensorMode_ );
        if (sensorMode_ > 0 && sensorMode_ <= 3 ) {
                Serial.print("Current sensorMode is: ");
                Serial.println(sensorMode);
                // Changing Sensor Mode
                sensorMode = sensorMode_;
                Serial.print("Updated sensorMode is: ");
                Serial.println(sensorMode);
        } else {
                Serial.println("Sensor Mode is the same.");
        }
}

void ModifySamplingRate(McpsIndication_t *mcpsIndication) {
        unsigned int sampling_rate = 0;
        sampling_rate =  (mcpsIndication->Buffer[4]) | ( sampling_rate );
        sampling_rate =  (mcpsIndication->Buffer[5]) | ( sampling_rate << 8);
        if (sampling_rate != 0 ) {
                Serial.print("Current sensor sampling rate is: ");
                Serial.println(sensor_sampling_rate);
                // Changing Sensor Mode
                sensor_sampling_rate = sampling_rate;
                Serial.print("Updated sensor sampling rate is: ");
                Serial.println(sensor_sampling_rate);
        } else {
                Serial.println("Sensor sampling rate is the same.");
        }
}

void ModifyNumberOfSamples(McpsIndication_t *mcpsIndication) {
        unsigned int numb_readings = 0;
        numb_readings =  (mcpsIndication->Buffer[6]) | ( numb_readings );
        if (numb_readings != 0 && numb_readings < 20) {
                Serial.print("Current number of readings per measurement: ");
                Serial.println(sensor_numberOfReadings);
                // Changing Sensor Mode
                sensor_numberOfReadings = numb_readings;
                Serial.print("Updated number of readings per measurement: ");
                Serial.println(sensor_numberOfReadings);
        } else {
                Serial.println("Sensor number of readings per measurement is the same.");
        }
}

void ModifySensorSettings(McpsIndication_t *mcpsIndication) {

        switch (mcpsIndication->BufferSize) {
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
                //should never reach here
                Serial.println("Invalid Sensor settings received.");
                break;
        }
}

void ModifyKeys(McpsIndication_t *mcpsIndication) {
        EEPROM.begin(512);
        EEPROM.write(isDeployed, byte('y'));
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
        EEPROM.write(selectionAddress, byte('n')); // Save the choice to EEPROM
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
                EEPROM.write(selectionAddress, byte('y'));
        }
}

uint8_t get_current_sensor_state(void) {
        return SENSOR_STATE;
}

void InitStoreKeys(void) {
        Serial.println("First time setup, Storing the original AppEUI...");
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
        Serial.println("Default Keys saved in the EEPROM.");
}

void process_operation(McpsIndication_t *mcpsIndication) {
        if ((char(mcpsIndication->Buffer[1]) == 's') && (char(mcpsIndication->Buffer[2]) == 't') && (char(mcpsIndication->Buffer[3]) == 'a') && (char(mcpsIndication->Buffer[4]) == 'r') && (char(mcpsIndication->Buffer[5]) == 't')) {
                Serial.println("Start sensing, uplinks are Ultrasonic measurements....");
                if(is_deployed == 'y'){
                        //save that the sensor is deployed and started
                        EEPROM.begin(512);
                        EEPROM.write(isStarted, byte('y'));
                        Serial.println("Saved the sensor state. From now uplinks will be readings even after reset.....");
                        EEPROM.end();
                }
                SENSOR_STATE = SENSING_STATE;
                appTxDutyCycle = TX_INTERVAL * 1000;
        } else if ((char(mcpsIndication->Buffer[1]) == 's') && (char(mcpsIndication->Buffer[2]) == 't') && (char(mcpsIndication->Buffer[3]) == 'o') && (char(mcpsIndication->Buffer[4]) == 'p')) {
                Serial.println("Stop sensing, uplinks are sensor CFG packets.");
                SENSOR_STATE = CFG_STATE;
                appTxDutyCycle = 10*1000;
        } else if ((char(mcpsIndication->Buffer[1]) == 'r') && (char(mcpsIndication->Buffer[2]) == 'e') && (char(mcpsIndication->Buffer[3]) == 's') && (char(mcpsIndication->Buffer[4]) == 'e') && (char(mcpsIndication->Buffer[5]) == 't')) {
                Serial.println("Reset Sensor.");
                SENSOR_STATE = RESET_STATE;
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
                ModifySensorSettings(mcpsIndication);
                Serial.println("Successfully modified sensor settings.");
                appTxDutyCycle = TX_INTERVAL*1000; //update appTxDutyCycle
                break;
        case 0x41:
                /* Sensor APP Change
                   Packet format:
                  |   oper     |   AppEUI(big-endian)   |   DevEUI(big-endian)  |   AppKEY(big-endian)    |
                  |   0x41     |         8 bytes        |          8 bytes      |         16 bytes        |
                 */
                Serial.println("\n\nAttempting to change TTN Application.....");
                if (mcpsIndication->BufferSize == 33) // 33: 0x41, AppEUI, AppKey, DevEUI
                {
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

void prepareMaxbotixFrame(){
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


        byte lowbyte, highbyte, lowbat, highbat;

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

void prepareRG15Frame(){
      if (counter_rg15 >= MAX_COUNTER_RG15){
                        Serial.println("Resetting Counter....");
                        counter_rg15 = 0;       //reset every 24 hrs or 1440 minutes
                }
                if (counter_rg15==0){
                        clearTotalAccRG15();   //clear ACK at every cycle including reset
                }
                Serial.print("Counter is: ");Serial.println(counter_rg15);
                String polledString;
                polledString = pollReadingFromRG15();
                polledString.trim();
                Serial.print("polled string: "); Serial.println(polledString);
                char charBuffer[140];
                polledString.toCharArray(charBuffer, 140);
                char delim[] = " ";

                char *ptr = strtok(charBuffer, delim);  // Extract the first word
                // Check if it is Acc
                char *acc;
                acc = strstr (charBuffer, "Acc");
                bool mmFound = false;
                float readings_array_pg15[4];
                if (*acc == 'A' && *(acc + 1) == 'c' && *(acc + 2) == 'c') {
                        int cntr = 0; // measurements counter; 4 total if Acc in line
                        while (ptr != NULL) {
                                char *foundDot = strstr (ptr, ".");
                                if (foundDot != NULL) {
                                        char *p = ptr;  // char pointer
                                        int ctr2 = 0;   // char counter
                                        char arr[6];    // arr to store reading 
                                        while (*p != NULL) {
                                                arr[ctr2] = *p;
                                                p++;
                                                ctr2++;
                                        }
                                        readings_array_pg15[cntr] = atof(arr) * 100; // convert char array to readings and *100 to remove decimals
                                        // Serial.println(readings_array_pg15[cntr]);
                                        cntr = cntr + 1;
                                }
                                ptr = strtok (NULL, " ");   // Iterate
                                if (!mmFound) {             // Check units
                                        if (*ptr == 'm' && *(ptr + 1) == 'm') {
                                                mmFound = true;
                                        }
                                }
                        }
                }
                appDataSize = 17;
                int idx = 0;
                // Serial.println("The readings are: ");
                for (int i = 0; i < 4; i++) {
                        // Serial.println(readings_array_pg15[i]);
                        appData[4 * i] = byte((uint32_t)readings_array_pg15[i] & 0xFF);
                        // Serial.println(appData[4 * i]);
                        appData[4 * i + 1] = byte(((uint32_t)readings_array_pg15[i] >> 8) & 0xFF);
                        // Serial.println(appData[4 * i + 1]);
                        appData[4 * i + 2] = byte(((uint32_t)readings_array_pg15[i] >> 16) & 0xFF);
                        // Serial.println(appData[4 * i + 2]);
                        appData[4 * i + 3] = byte(((uint32_t)readings_array_pg15[i] >> 32) & 0xFF);
                        // Serial.println(appData[4 * i + 3]);
                }
                // Serial.println();
                if (mmFound) {
                        Serial.println("Units are mm.");
                        appData[16] = byte('m');
                } else {
                        Serial.println("Units are in.");
                        // appData[16] = byte('i');
                }
                counter_rg15++;  
}

void prepareTippingBucketFrame(){
        byte lowbyte, highbyte;
        appDataSize = 17 + 2;
        init_tipping_bucket(60, 5);
        bucket_tip_count = tip_count_looper();
        Serial.print(bucket_tip_count); Serial.println(" tips counted in last period");
        lowbyte = lowByte(bucket_tip_count);
        highbyte = highByte(bucket_tip_count);
        appData[0] = (unsigned char)ERROR_FLAGS;
        appData[17] = (unsigned char)lowbyte;
        appData[18] = (unsigned char)highbyte;
}

/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port ){
        /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
           appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
           if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
           if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
           for example, if use REGION_CN470,
           the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
         */
        Serial.println("Preparing TX frame...");
        

        // Error flags
        ERROR_FLAGS = 0x00;

        switch (SENSOR_STATE)
        {
        case SENSING_STATE:
        {
                // Cannot use Maxbotix AND other sensors as distance values would be overwritten
                #ifdef USE_MAXBOTIX
                prepareMaxbotixFrame();
                #endif

                #ifdef USE_RG15
                prepareRG15Frame();
                #endif

                #ifdef USE_TIPPING_BUCKET
                prepareTippingBucketFrame();
                #endif

                break;
        }

        case CFG_STATE:
        {
                /*
                   CFG update uplink Format:
                 | Error Flag  |   sensor_sleep   |    sensor_agg     |   sensor_meas_delta     | sensor_reading_count   |    sensor_state   |    fw_ver       |
                 |    255 (FF) |     2 bytes      |      1 byte       |          2 bytes        |          1 byte        |        1 byte     |    6 bytes      |

                  Sensor State:
                  |  Start  |   Stop  |  Reset  |
                  |   's'   |   'x'   |   'r'   |

                 */
                byte lowbyte, highbyte;
                appDataSize = 11;
                ERROR_FLAGS = 255;
                appData[0] = (unsigned char)ERROR_FLAGS;

                // sensor_sleep
                byte lowduty = lowByte(TX_INTERVAL);
                byte highduty = highByte(TX_INTERVAL);
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
                appData[10] = (unsigned char)PATCH_VERSION;

                break;
        }

        default:
        {
                //should never reach here
                SENSOR_STATE = SENSING_STATE;
                break;
        }
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
        /* On reset check if sensor has been deployed.

                            no
                If deployed -> CFG_STATE
                        | yes
                        ^
                        if started -> SENSOR_STATE is SENSING_STATE

        */
        TX_INTERVAL = packet_interval;
        EEPROM.begin(512);
        is_deployed = char(EEPROM.read(isDeployed));
        is_started = char(EEPROM.read(isStarted));
        EEPROM.end();
        Serial.println(F("Checking previously known sensor state and setting up dutycycle....."));
        if (is_deployed == 'y') {
                Serial.println("\n\n=========================================================");
                Serial.println("Sensor is deployed!\n");
                Serial.print("Reset cause CY_SYS_RESET_WDT: "); Serial.println(CY_SYS_RES_CAUSE_REG & (CY_SYS_RESET_WDT) ); // If True - 1
                Serial.print("Reset cause CY_SYS_RESET_PROTFAULT: "); Serial.println(CY_SYS_RES_CAUSE_REG & (CY_SYS_RESET_PROTFAULT)); // If true - 8
                Serial.print("Reset cause CY_SYS_RESET_SW: "); Serial.println(CY_SYS_RES_CAUSE_REG & (CY_SYS_RESET_SW)); // If true - 16
                if (is_started == 'y') { //deployed and started
                        Serial.println("\nSensor is started! Uplinks are readings...");
                        SENSOR_STATE = SENSING_STATE;
                        appTxDutyCycle = TX_INTERVAL*1000;
                } else { //deployed but not started
                        Serial.println("\nSensor is not started yet...! Uplinks are CFG packets...");
                        SENSOR_STATE = CFG_STATE;
                        appTxDutyCycle = 10 *1000;
                }
                Serial.println("=========================================================\n\n");
        } else { //not deployed
                SENSOR_STATE = CFG_STATE;
                appTxDutyCycle = 10 *1000;
        }
        Serial.print(F("\n\nDutycycle is set to ")); Serial.print(TX_INTERVAL); Serial.println(F(" seconds.\n\n"));
        Serial.println(F("Checking keys..."));
        EEPROM.begin(512);
        // Check init_ for 'y' or 'n'
        char init_ = char(EEPROM.read(init_Address));
        char useOrigApp = char(EEPROM.read(selectionAddress));
        if ( init_ == 'y') {
                // already initialized and has a new key
                if (useOrigApp == 'n' || useOrigApp == 'N') {
                        // Load new AppEUI
                        Serial.println("Already initialized and have new keys.");
                        LoadNewKeys();
                } else {
                        Serial.println("Using default Keys...");
                        // check if any of them are zeroes and load from EEPROM if needed
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
        deviceState = DEVICE_STATE_INIT;
        LoRaWAN.ifskipjoin(); // downlinks fail if this is not called!

}

void lorawan_runloop_once(void)
{
        switch ( deviceState )
        {
        case DEVICE_STATE_INIT:
        {
                printDevParam();
                LoRaWAN.init(loraWanClass, loraWanRegion);
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
                if (SENSOR_STATE == RESET_STATE) {
                        // Reset
                        innerWdtEnable(false);
                        delay(5000); //Wait until the MCU resets
                }
                Serial.print("Going to sleep, next uplink in "); Serial.print(TX_INTERVAL); Serial.println(" s.");
                deviceState = DEVICE_STATE_SLEEP;
                break;
        }
        case DEVICE_STATE_SLEEP:
        {
                CySysWdtDisable();
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
