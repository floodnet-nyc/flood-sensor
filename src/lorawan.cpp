#include "lorawan.h"
#include "featherwing.h"
#include "functions.h"
#include "sleep.h"
#include "lmicpinmappings.h"
#include "sensorcfg.h"
#include "maxbotix.h"
#include <math.h>
#include <FlashStorage.h>

#define LORA_TIMEOUT 20000L

unsigned int TX_INTERVAL;         // dutyCycle
int joinFailCounter = 0;          // counter for Join failure

bool GO_TO_SLEEP = false;         // Set to false on start and after sleep; is set to true when IDLE
bool ACTIVE_TTN_SESSION = false;
uint16_t distance;
u4_t sequence_up,sequence_dn,last_sequence_up,last_sequence_dn;
float measuredvbat;               // Measured Battery Level in mVolts
uint16_t batlevel;
unsigned int ERROR_FLAGS;
String event_ev;
u4_t netid = 0;
devaddr_t devaddr = 0;
u1_t nwkKey[16];
u1_t artKey[16];
void os_getArtEui (u1_t* buf) {
    memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui (u1_t* buf) {
    memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey (u1_t* buf) {
    memcpy_P(buf, APPKEY, 16);
}

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:   Serial.println("EV_SCAN_TIMEOUT");     break;
        case EV_BEACON_FOUND:   Serial.println ("EV_BEACON_FOUND");    break;
        case EV_BEACON_MISSED:  Serial.println("EV_BEACON_MISSED");    break;
        case EV_BEACON_TRACKED: Serial.println("EV_BEACON_TRACKED");   break;
        case EV_JOINING:        Serial.println("EV_JOINING");          break;
        case EV_RFU1:           Serial.println("EV_RFU1");             break;
        case EV_JOIN_FAILED:    Serial.println ("EV_JOIN_FAILED");     break;
        case EV_REJOIN_FAILED:  Serial.println("EV_REJOIN_FAILED");    break;
        case EV_LOST_TSYNC:     Serial.println("EV_LOST_TSYNC");       break;
        case EV_RESET:          Serial.println("EV_RESET");            break;
        case EV_RXCOMPLETE:     Serial.println("EV_RXCOMPLETE");       break;
        case EV_LINK_DEAD:      Serial.println("EV_LINK_DEAD");        break;
        case EV_LINK_ALIVE:     Serial.println("EV_LINK_ALIVE");       break;
        case EV_SCAN_FOUND:     Serial.println("EV_SCAN_FOUND");       break;
        case EV_TXSTART:        Serial.println("EV_TXSTART");          break;
        case EV_TXCOMPLETE:
            Serial.println("EV_TXCOMPLETE ");
            if (LMIC.txrxFlags & TXRX_ACK) {
                Serial.println("with ACK");
            }
            if (LMIC.dataLen) {
                Serial.print(F("Received "));
                Serial.print(LMIC.dataLen);
                Serial.print(" bytes of payload: 0x");
                for (int i = 0; i < LMIC.dataLen; i++) {
                    if (LMIC.frame[LMIC.dataBeg + i] < 0x10) {
                        Serial.print(F("0"));
                    }
                    Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
                }
                Serial.println();
                //process_received_downlink();
            }
            GO_TO_SLEEP = true; //LMIC done!
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            event_ev = String(event_ev + "EV_JOINED" );
            writeToSDCard(event_ev);
            // Stop blinking the LED and stop blinkLEDjob
            //os_clearCallback(&blinkLEDjob);
            digitalWrite(LED_BUILTIN, LOW);
        {

            LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
            Serial.print("netid: ");
            Serial.println(netid, DEC);
            Serial.print("devaddr: ");
            Serial.println(devaddr, HEX);
            Serial.print("AppSKey: ");
            for (size_t i = 0; i < sizeof(artKey); ++i) {
                if (i != 0)
                    Serial.print("-");
                printHex2(artKey[i]);
            }
            Serial.println("");
            Serial.print("NwkSKey: ");
            for (size_t i = 0; i < sizeof(nwkKey); ++i) {
                if (i != 0)
                    Serial.print("-");
                printHex2(nwkKey[i]);
            }
            Serial.println();
        }
            // Disable link check validation (automatically enabled
            // during join, but because slow data rates change max TX
            // size, we don't use it.
            LMIC_setLinkCheckMode(0);
            break;
        default:
            Serial.println("Unknown event");
            break;
    }
}

static void lmicInit (osjob_t* j) {
    /*onEvent() callback function will be invoked
     by the scheduler later on for the events EV_JOINING,
     EV_JOINED or EV_JOIN_FAILED.*/

    // Reset the MAC state. Session and pending data transfers will be discarded.

    LMIC_reset();
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    // Disable link check validation (automatically enabled)
    LMIC_setLinkCheckMode(0);
    LMIC_setDrTxpow(DR_SF7, 14);
    LMIC_selectSubBand(1);
     digitalWrite(13, LOW);
}

void lmicSaveTTNSession(void){
    sequence_up = LMIC.seqnoUp;
    sequence_dn = LMIC.seqnoDn;
}

void lmicLoadTTNSession(void){
    LMIC_setSession(netid, devaddr, nwkKey, artKey);
    LMIC.seqnoUp = last_sequence_up;
    LMIC.seqnoDn = last_sequence_dn;
}

bool joinedTTN(void){  // If not joined TTN join and return a boolean true
    if(!ACTIVE_TTN_SESSION){
        Serial.println(F("Connecting to TTN..."));
        digitalWrite(13, HIGH);
        setupLMIC();
        Serial.println(F("Setup Done"));

        LMIC_startJoining();//Failure is being caused by this function lmic library page 13
        //If the above line is commented out then flow falls into an infinte loop maybe we have to check for flow too.
        Serial.println(F("started  Joining"));
        digitalWrite(13, LOW);
        if (!LMIC_startJoining) {
            ACTIVE_TTN_SESSION = true;
            lmicSaveTTNSession();   // Save the TTN session after a successful join.
        }
    }
    return ACTIVE_TTN_SESSION;
}

 void setupLMIC(){
    /*
        Function to Setup LMIC can be used for the following:
            - init LMIC for the first time
            - reset LMIC: When called after an init of LMIC, This function resets the
              LMIC initialization and sets up initial job lmicInit
     */
     digitalWrite(13, HIGH);
    // initialize run-time env
    os_init();
    // setup initial job to bootstrap protocol actions and generate events
    os_setCallback(&initjob, lmicInit);
}
void prepare_cfg_packet(void){
  byte lowbyte, highbyte, lowbat, highbat;
  String packet_data;
  // Send Sensor Config via Uplink
  /*
  CFG update uplink Format:
        | Error Flag  | Sensor Mode | Sensor Sampling Rate | Sensor Number of Readings |
        |    255 (FF) |    1 byte   |      2 bytes         |        1 bytes            |
  */
  ERROR_FLAGS = 255;
  cfg_packet[0] = (unsigned char)ERROR_FLAGS;
  packet_data = String("CFG Update via Uplink");
  writeToSDCard(packet_data);
  byte lowduty = lowByte(TX_INTERVAL);
  byte highduty = highByte(TX_INTERVAL);
  cfg_packet[1] = (unsigned char)lowduty;
  cfg_packet[2] = (unsigned char)highduty;
  cfg_packet[3] = (unsigned char)sensorMode;
  lowbyte = lowByte(sensor_sampling_rate);
  highbyte = highByte(sensor_sampling_rate);
  cfg_packet[4] = (unsigned char)lowbyte;
  cfg_packet[5] = (unsigned char)highbyte;
  cfg_packet[6] = (unsigned char)sensor_numberOfReadings;
}
void prepare_lora_packet(void) {

  byte lowbyte, highbyte, lowbat, highbat;
  String packet_data;
  // Error
  ERROR_FLAGS = pow(2,0)* SD_ERROR;

  // Formatting payload

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



    // Maxbotix
    distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" mm");
    packet_data = String("Distance in mm is: ") + String(distance);
    writeToSDCard(packet_data);

    // Battery
    measuredvbat = analogRead(VBATPIN); //Float
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Serial.print("VBat: " ); Serial.println(measuredvbat);
    measuredvbat *= 1000; //make it milli volts to transmit
    Serial.print("VBat in mVolts: " ); Serial.println(measuredvbat);
    batlevel = measuredvbat; //Payload
    packet_data = String("Battery Level in mVolts is: ") + String(batlevel);
    writeToSDCard(packet_data);

    // Payload
    lowbat = lowByte(batlevel);
    highbat = highByte(batlevel);
    lora_packet[0] = (unsigned char)ERROR_FLAGS;
    packet_data = String("SD Error flag is: ") + String(SD_ERROR);
    writeToSDCard(packet_data);
    lora_packet[1] = (unsigned char)lowbat; //we're unsigned
    lora_packet[2] = (unsigned char)highbat;
    lowbyte = lowByte(distance);
    highbyte = highByte(distance);
    lora_packet[3] = (unsigned char)lowbyte;
    lora_packet[4] = (unsigned char)highbyte;
  }


void execRuntimeJobs(void){
    os_runloop_once();
}

lmic_tx_error_t lmic_tx_retVAL;
/*
 | Name                       | Value | Description                                                                                                                                                                                        |
 |----------------------------|-------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
 | LMIC_ERROR_SUCCESS         | 0     | No error occurred, EV_TXCOMPLETE will be posted.                                                                                                                                                   |
 | LMIC_ERROR_TX_BUSY         | -1    | The LMIC was busy sending another message. This message was not sent. EV_TXCOMPLETE will not be posted for this message.                                                                           |
 | LMIC_ERROR_TX_TOO_LARGE    | -2    | The queued message is too large for any data rate for this region. This message was not sent.  EV_TXCOMPLETE will not be posted for this message.                                                  |
 | LMIC_ERROR_TX_NOT_FEASIBLE | -3    | The queued message is not feasible for the current data rate. This message was not sent. EV_TXCOMPLETE will not be posted for this message.                                                        |
 | LMIC_ERROR_TX_FAILED       | -4    | The queued message failed for some other reason than data length, during the initial call to the LMIC to transmit it. This message was not sent. EV_TXCOMPLETE will not be posted for this message |
 */


bool sendUplink(unsigned char txPacket[]){
    // Join TTN if already not joined
    // If joined use current session


    if (joinedTTN()) {
        uint64_t start = millis();
        while (true) {
            switch (LMIC_setTxData2_strict(1, txPacket, sizeof(txPacket), 0)) {
                case LMIC_ERROR_SUCCESS:
                    // uplink success: message has been sent.
                    Serial.println("Uplink successful.");
                    return true;
                    break;
                case LMIC_ERROR_TX_BUSY:   // Already has a TX job that is pending
                case LMIC_ERROR_TX_FAILED: // Uplink not sent
                    execRuntimeJobs();
                    break;
                default:
                    // for other errors, message is not sent
                    break;
            }
             if (millis() - start > LORA_TIMEOUT) {
                Serial.println("LoRa Timedout. Resetting the LMIC stack...");
                setupLMIC();
                Serial.println("LMIC is reset.");
                lmicLoadTTNSession();
                Serial.println("Loading the previous LMIC state...");
                return false;
            }
        }
    }
}
