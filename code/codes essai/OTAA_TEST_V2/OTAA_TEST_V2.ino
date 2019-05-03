//Librairies
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <Wire.h>
#include "LowPower.h"
#include <SoftwareSerial.h>

//software serial definition
SoftwareSerial mini(4,3); //Rx Tx

String donne;
int counter = 0;

/// OTAA SETUP
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

//functions

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

void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {


  } 

    
    char donne[130];
    
    LMIC_setTxData2(1, donne, sizeof(donne), 0);
  }

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

}
