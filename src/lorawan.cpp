#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "sensorcfg.h"
#include "maxbotix.h"

/*
 set LoraWan_RGB to Active,the RGB active in loraWan
 RGB red means sending;
 RGB purple means joined done;
 RGB blue means RxWindow1;
 RGB yellow means RxWindow2;
 RGB green means received done;
 */

/* OTAA para*/
uint8_t devEui[] = {  };
uint8_t appEui[] = {  };
uint8_t appKey[] = {  };

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

/*OTAA or ABP*/
bool overTheAirActivation = LORAWAN_NETMODE;

/*ADR enable*/
bool loraWanAdr = LORAWAN_ADR;

/* set LORAWAN_Net_Reserve ON, the node could save the network info to flash, when node reset not need to join again */
bool keepNet = LORAWAN_NET_RESERVE;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed = LORAWAN_UPLINKMODE;

/* Application port */
uint8_t appPort = 1;
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

void onSleep()
{
    Serial.printf("Going into lowpower mode, %d s later wake up.\r\n", timetillwakeup/1000);
    lowpower = 1;
    //  Radio.Sleep( );
    //timetillwakeup ms later wake up;
    TimerSetValue( &wakeUp, timetillwakeup );
    TimerStart( &wakeUp );
}

void onWakeUp()
{
    Serial.printf("\r\nWoke up, %d s later.\r\n", timetillwakeup/1000);
    lowpower = 0;
    //  //timetillsleep ms later into lowpower mode;
    //  TimerSetValue( &sleep, timetillsleep );
    //  TimerStart( &sleep );
}

uint16_t distance;
uint16_t batlevel;

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
    digitalWrite(Vext, LOW);
    // Maxbotix
    distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" mm");

    // Battery
    pinMode(VBAT_ADC_CTL,OUTPUT);
    digitalWrite(VBAT_ADC_CTL,LOW);
    batlevel=analogRead(ADC)*2;
    /*
     * Board, BoardPlus, Capsule, GPS and HalfAA variants
     * have external 10K VDD pullup resistor
     * connected to GPIO7 (USER_KEY / VBAT_ADC_CTL) pin
     */
    pinMode(VBAT_ADC_CTL, INPUT);

    appDataSize = 5;
    // First byte is zero to match decoder on backend
    appData[0] = 0x00;

    appData[1] = lowByte(batlevel);
    appData[2] = highByte(batlevel);
    appData[3] = lowByte(distance);
    appData[4] = highByte(distance);
    digitalWrite(Vext, HIGH);
    onSleep();
}


void setup_lorawan(unsigned int packet_interval) {
    appTxDutyCycle = packet_interval*1000;
    boardInitMcu();
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, HIGH);
    Serial.begin(115200);
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
            txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND ); // default value of APP_TX_DUTYCYCLE_RND is 1000
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
