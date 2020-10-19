#include "lorawan.h"
#include "featherwing.h"
#include "functions.h"
#include "sleep.h"
#include "lmicpinmappings.h"
#include "sensorcfg.h"
#include "maxbotix.h"

static osjob_t sendjob;


unsigned int TX_INTERVAL;


unsigned char lora_packet[5];
bool TX_COMPLETED = false;


void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}

void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}

void lmicsetup( unsigned int packet_interval = 300) {       //Future setup variables
  Serial.println(F("Setting up LoraWAN..."));

  digitalWrite(13, HIGH);
  // LMIC init
  os_init();
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  //LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  // Disable link check validation (automatically enabled)
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7, 14);
  LMIC_selectSubBand(1);
  digitalWrite(13, LOW);
  Serial.println("Setup Ready!");
  TX_INTERVAL = packet_interval;
  // Start job (sending automatically starts OTAA too)
  Serial.println("Starting first job in setup");
  prepare_packet();
  do_send(&sendjob);
}

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
    String do_sendstr1 = "OP_TXRXPEND, not sending";
    writeToSDCard(do_sendstr1);
  } else {
    // Prepare upstream data transmission at the next possible time.
    int lmic_tx_retVAL = LMIC_setTxData2(1, lora_packet, sizeof(lora_packet), 0);
    String do_sendstr;
    if (lmic_tx_retVAL == 0) {
      Serial.println(F("Packet queued and lmic_tx_retVAL is 0."));
    } else {
      do_sendstr = String ("Something is wrong ") + String("Error number: ") + String(lmic_tx_retVAL);
      Serial.println(do_sendstr);
      writeToSDCard(do_sendstr);
    }
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void onEvent (ev_t ev) {
  Serial.print(os_getTime());
  Serial.print(": ");
  String event_ev;
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      event_ev = String(event_ev + "EV_SCAN_TIMEOUT");
      writeToSDCard(event_ev);
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      event_ev = String(event_ev + "EV_BEACON_FOUND");
      writeToSDCard(event_ev);
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      event_ev = String(event_ev + "EV_BEACON_MISSED");
      writeToSDCard(event_ev);
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      event_ev = String(event_ev + "EV_BEACON_TRACKED" );
      writeToSDCard(event_ev);
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      event_ev = String(event_ev + "EV_JOINING");
      writeToSDCard(event_ev);
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      event_ev = String(event_ev + "EV_JOINED" );
      writeToSDCard(event_ev);
      {
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
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
      // size, we don't use it in this example.
      LMIC_setLinkCheckMode(0);
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_RFU1:
      ||     Serial.println(F("EV_RFU1"));
      ||     break;
    */
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      event_ev = String(event_ev + "EV_JOIN_FAILED");
      writeToSDCard(event_ev);
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      event_ev = String(event_ev + "EV_REJOIN_FAILED" );
      writeToSDCard(event_ev);
      break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      Serial.println("");
      Serial.println("");
      event_ev = String(event_ev + "EV_TXCOMPLETE (includes waiting for RX windows)");
      writeToSDCard(event_ev);
      if (LMIC.txrxFlags & TXRX_ACK)
        Serial.println(F("Received ack"));
      event_ev = String("Received ack");
      writeToSDCard(event_ev);
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.print(LMIC.dataLen);
        Serial.print(" bytes of payload: 0x");
        for (int i = 0; i < LMIC.dataLen; i++) {
          if (LMIC.frame[LMIC.dataBeg + i] < 0x10) {
            Serial.print(F("0"));
          }
          Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
        }
        Serial.println();
        process_received_downlink();
      }
      TX_COMPLETED = true;
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      event_ev = String(event_ev + "EV_LOST_TSYNC" );
      writeToSDCard(event_ev);
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      event_ev = String(event_ev + "EV_RESET");
      writeToSDCard(event_ev);
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      event_ev = String(event_ev + "EV_RXCOMPLETE");
      writeToSDCard(event_ev);
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      event_ev = String(event_ev + "EV_LINK_DEAD");
      writeToSDCard(event_ev);
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      event_ev = String(event_ev + "EV_LINK_ALIVE");
      writeToSDCard(event_ev);
      break;
    /*
      || This event is defined but not used in the code. No
      || point in wasting codespace on it.
      ||
      || case EV_SCAN_FOUND:
      ||    Serial.println(F("EV_SCAN_FOUND"));
      ||    break;
    */
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      event_ev = String(event_ev + "EV_TXSTART");
      writeToSDCard(event_ev);
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      event_ev = String(event_ev + "EV_TXCANCELED");
      writeToSDCard(event_ev);
      break;
    case EV_RXSTART:
      /* do not print anything -- it wrecks timing */
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      event_ev = String(event_ev + "EV_JOIN_TXCOMPLETE: no JoinAccept" );
      writeToSDCard(event_ev);
      break;
    default:
      Serial.print(F("Unknown event: "));
      event_ev = String(event_ev + "Unknown event");
      Serial.println((unsigned) ev);
      writeToSDCard(event_ev);
      break;
  }
}



void process_received_downlink(void) {

  String str_downlink = String("Processing received downlink....");
  writeToSDCard(str_downlink);
  unsigned long dutycycle = 0;
  Serial.print("Current duty cycle is: ");
  Serial.println(TX_INTERVAL);
  str_downlink = String("Current duty cycle is: ") + String(TX_INTERVAL);
  writeToSDCard(str_downlink);
  for (int i = 0; i < LMIC.dataLen; i++) {
    if (LMIC.frame[LMIC.dataBeg + i] < 0x10) {
      //Serial.print(F("0"));
    }
     //Serial.println(LMIC.frame[LMIC.dataBeg + i]);
     dutycycle =  (LMIC.frame[LMIC.dataBeg + i]) | ( dutycycle << 8*i);
  }
  //Serial.println();
  // Changing Duty Cycle
  TX_INTERVAL = dutycycle;
  Serial.print("Updated dutycycle is: ");
  Serial.println(TX_INTERVAL);
  str_downlink = String("Updated dutycycle is: ") + String(TX_INTERVAL);
  writeToSDCard(str_downlink);
}

uint16_t distance;
// Measured Battery Level in mVolts
float measuredvbat;
uint16_t batlevel;

void prepare_packet(void) {

  /* LoraWAN packet format
          | Error flags  | Battery Level | Ultrasonic reading  |
          |   1 byte     |    2 bytes    |        2 bytes      |

          |     Ultrasonic reading      |
          |           2 bytes           |
          |    high byte | low byte     |

          |       Battery Level       |
          |           2 bytes         |
          |    high byte | low byte   |

          |---------------------------------------------Error Flags  ------------------------------------------------------------|
          |     bit 7    |     bit 6   |     bit 5    |     bit 4    |     bit 3    |     bit 2    |     bit 1    |     bit 0    |
          |              |             |              |              |              |              |              | SD error flag|
  */

  byte lowbyte, highbyte, lowbat, highbat;

  // Maxbotix
  distance = read_sensor_using_modes(sensorMode, sensor_sampling_rate, sensor_numberOfReadings);
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
  String packet_data = String("Distance in mm is: ") + String(distance);
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

  // Formatting payload
  lowbyte = lowByte(distance);
  highbyte = highByte(distance);
  lora_packet[0] = (unsigned char)lowbyte;
  lora_packet[1] = (unsigned char)highbyte;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" mm");
  lowbat = lowByte(batlevel);
  highbat = highByte(batlevel);
  lora_packet[2] = (unsigned char)lowbat; //we're unsigned
  lora_packet[3] = (unsigned char)highbat;
  lora_packet[4] = (unsigned char)sensorMode;
  packet_data = String("Sensor Mode is: ") + String(sensorMode);
  writeToSDCard(packet_data);
  lora_packet[5] = (unsigned char)SD_ERROR;
  packet_data = String("SD Error flag is: ") + String(SD_ERROR);
  writeToSDCard(packet_data);

}

void lorawan_runloop_once() {
  os_runloop_once();
  if ( !os_queryTimeCriticalJobs(ms2osticksRound((TX_INTERVAL * 1000) - 1000 )) && TX_COMPLETED == true) {
    TX_COMPLETED = false;
    // This means the previous TX is complete and also no Critical Jobs pending in LMIC
    Serial.println("About to go to deep sleep and no critical jobs");
    gotodeepsleepnow(TX_INTERVAL);
    Serial.println("Im awake and TX_COMPLETED is set to false");
    // Prepare a packet in relaxed setiing
    prepare_packet();
    os_setCallback(&sendjob, do_send);
  }
}
