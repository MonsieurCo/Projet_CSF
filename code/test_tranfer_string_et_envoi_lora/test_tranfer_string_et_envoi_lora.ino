#include <SPI.h>
#include <LoRa.h>
#include <SoftwareSerial.h>
SoftwareSerial mini(4,3); //Rx Tx

String donne;
int counter = 0;

// Parameters you can play with :

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
 // you can change the payload

#define SS 10
#define RST 8
#define DI0 2
#define BAND 866.5E6  // Here you define the frequency carrier

void setup() {
   mini.begin(9600);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS,RST,DI0);

  

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength);
// LoRa.setPolarity(1);
 //LoRa.setFSK(); 
 
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
    
    if ( donne.length() >= 77 )
    {
       LoRa.beginPacket();
  LoRa.print(donne);  

  LoRa.endPacket();
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
  
 
