#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include "LowPower.h"
#include <SoftwareSerial.h>
SoftwareSerial mini(4,3); //Rx Tx

String donne;
int counter = 0;




#define debugSerial Serial
#define SHOW_DEBUGINFO
#define debugPrintLn(...) { if (debugSerial) debugSerial.println(__VA_ARGS__); }
#define debugPrint(...) { if (debugSerial) debugSerial.print(__VA_ARGS__); }





static const u1_t PROGMEM APPEUI[8] = { 0xE0, 0x33, 0x01, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getArtEui (u1_t* buf) {
  memcpy_P(buf, APPEUI, 8);
}

static const u1_t PROGMEM DEVEUI[8] = { 0x69, 0x00, 0x00, 0x00, 0x00, 0x06, 0xCE, 0x21 };
void os_getDevEui (u1_t* buf) {
  memcpy_P(buf, DEVEUI, 8);
}


static const u1_t PROGMEM APPKEY[16] = { 0xE2, 0x6C, 0x6A, 0x69, 0x85, 0x40, 0xE8, 0x33, 0xA9, 0x6F, 0xBC, 0x05, 0xFD, 0x75, 0xCC, 0xB6 };
void os_getDevKey (u1_t* buf) {
  memcpy_P(buf, APPKEY, 16);
}


static osjob_t sendjob;


// ---------------------------------------------------------------------------------
// Functions
// ---------------------------------------------------------------------------------


// Schedule TX every this many seconds (might become longer due to duty
// cycle limitations).
unsigned int TX_INTERVAL = 300;

void setDataRate() {
  switch (LMIC.datarate) {
    case DR_SF12:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF12"));
    #endif      
      TX_INTERVAL = 4800;
      break;
    case DR_SF11: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF11"));
    #endif
      TX_INTERVAL = 2400;
      break;
    case DR_SF10: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF10"));
    #endif
      TX_INTERVAL = 1200;
      break;
    case DR_SF9: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF9"));
    #endif
      TX_INTERVAL = 600;
      break;
    case DR_SF8: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF8"));
    #endif
      TX_INTERVAL = 360;
      break;
    case DR_SF7: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF7"));
    #endif
      TX_INTERVAL = 180;
      break;
    case DR_SF7B: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: SF7B"));
    #endif
      TX_INTERVAL = 180;
      break;
    case DR_FSK: 
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("Datarate: FSK"));
    #endif
      TX_INTERVAL = 180;
      break;
    default: debugPrint(F("Datarate Unknown Value: "));
      debugPrintLn(LMIC.datarate); TX_INTERVAL = 600;
      break;
  }
}

extern volatile unsigned long timer0_millis;
void addMillis(unsigned long extra_millis) {
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis += extra_millis;
  SREG = oldSREG;
  sei();
}

void do_sleep(unsigned int sleepyTime) {
  unsigned int eights = sleepyTime / 8;
  unsigned int fours = (sleepyTime % 8) / 4;
  unsigned int twos = ((sleepyTime % 8) % 4) / 2;
  unsigned int ones = ((sleepyTime % 8) % 4) % 2;

#ifdef SHOW_DEBUGINFO
  debugPrint(F("Sleeping for "));
  debugPrint(sleepyTime);
  debugPrint(F(" seconds = "));
  debugPrint(eights);
  debugPrint(F(" x 8 + "));
  debugPrint(fours);
  debugPrint(F(" x 4 + "));
  debugPrint(twos);
  debugPrint(F(" x 2 + "));
  debugPrintLn(ones);
  
#endif


  for ( int x = 0; x < eights; x++) {
    // put the processor to sleep for 8 seconds
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
  for ( int x = 0; x < fours; x++) {
    // put the processor to sleep for 4 seconds
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);
  }
  for ( int x = 0; x < twos; x++) {
    // put the processor to sleep for 2 seconds
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
  }
  for ( int x = 0; x < ones; x++) {
    // put the processor to sleep for 1 seconds
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  }
  addMillis(sleepyTime * 1000);
}






void onEvent (ev_t ev) {
  #ifdef SHOW_DEBUGINFO
  Serial.print(os_getTime());
  Serial.print(": ");
  #endif
  switch (ev) {
    case EV_SCAN_TIMEOUT:
    #ifdef SHOW_DEBUGINFO
  debugPrintLn(F("EV_SCAN_TIMEOUT"));
  #endif
     
      break;
    case EV_BEACON_FOUND:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_BEACON_FOUND"));
    #endif      
      break;
    case EV_BEACON_MISSED:
      //debugPrintLn(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      //debugPrintLn(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_JOINING"));
    #endif      
      break;
    case EV_JOINED:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_JOINED"));
    #endif      
      setDataRate();      
      // Ok send our first data in 10 ms
      os_setTimedCallback(&sendjob, os_getTime() + ms2osticks(10), do_send);
      break;
    case EV_RFU1:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_RFU1"));
    #endif
      
      break;
    case EV_JOIN_FAILED:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_JOIN_FAILED"));
    #endif
      
void  lmicStartup(); //Reset LMIC and retry
      break;
    case EV_REJOIN_FAILED:
    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_REJOIN_FAILED"));
    #endif
      
      lmicStartup(); //Reset LMIC and retry
      break;
    case EV_TXCOMPLETE:

    #ifdef SHOW_DEBUGINFO
    debugPrintLn(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
    #endif
      
      if (LMIC.txrxFlags & TXRX_ACK)
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("Received ack"));
      #endif
        
      if (LMIC.dataLen) {
        #ifdef SHOW_DEBUGINFO
        debugPrintLn(F("Received "));
        debugPrintLn(LMIC.dataLen);
        debugPrintLn(F(" bytes of payload"));
        #endif 
       
      }
            // Schedule next transmission
      setDataRate();
      do_sleep(TX_INTERVAL);
      os_setCallback(&sendjob, do_send);
      break;
    case EV_LOST_TSYNC:
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("EV_LOST_TSYNC"));
      #endif      
      break;
    case EV_RESET:
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("EV_RESET"));
      #endif        
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("EV_RXCOMPLETE"));
      #endif      
      break;
    case EV_LINK_DEAD:
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("EV_LINK_DEAD"));
      #endif       
      break;
    case EV_LINK_ALIVE:
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("EV_LINK_ALIVE"));
      #endif       
      break;
    default:
      #ifdef SHOW_DEBUGINFO
      debugPrintLn(F("Unknown event"));
      #endif      
      break;
  }
}


void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    debugPrintLn(F("OP_TXRXPEND, not sending"));
  } 

    
    char donne[130];
    
    LMIC_setTxData2(1, donne, sizeof(donne), 0);
    debugPrintLn(F("PQ")); //Packet queued
  }
  // Next TX is scheduled after TX_COMPLETE event.


void lmicStartup() {
  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  LMIC_setLinkCheckMode(1);
  LMIC_setAdrMode(1);
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100); // Increase window time for clock accuracy problem
  

  // Start job (sending automatically starts OTAA too)
  // Join the network, sending will be
  // started after the event "Joined"
  LMIC_startJoining();
}

void setup() {
  mini.begin(9600);
  Serial.begin(9600);
  
  
  #ifdef SHOW_DEBUGINFO
  debugPrintLn(F("Starting"));
  #endif

  // LMIC init
  os_init();
  lmicStartup();

}

void loop() {
  while(mini.available())
  {
    char c = mini.read();
    
   if ( c == 'E') 
    {
     donne ="";
    }
    donne+= c;
    
    if ( donne.length() >= 119 )
    {
    os_runloop_once();
  counter++;

  Serial.print("Sending packet with payload {");
  Serial.print(donne);
  Serial.print("} N°");
  Serial.println(counter);
 
  
      donne ="";
      
      
    }
    delay(10);
 }

  os_runloop_once();
}
