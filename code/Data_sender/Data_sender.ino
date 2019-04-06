#include <Wire.h>
#include <SI7021.h>
#include <SPI.h>
#include <LoRa.h>
#define LAPIN A0 // PIN with Light sensor analog output 
#define LPPIN 4 // PIN with Light power input
int counter = 0;
SI7021 sensor;
int txPower = 20; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
#define SS 10
#define RST 8
#define DI0 2
#define BAND 866.5E6  // Nous avons choisi la fréquence 866,5 MHz 



static float light;

void setup() {
  //set up lumière
  Serial.begin(115200);
  pinMode(LPPIN, OUTPUT);
  //temprature humidité
  sensor.begin();

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



float readLight() {
  float result;
  // Light sensor Voltage
digitalWrite(LPPIN, HIGH); // Power the sensor
delay(10);
int sensorValue = analogRead(LAPIN);
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 3.3V):
  float voltage = sensorValue * (3.3 / 1023.0); // Batvalue is 3.3V
result = voltage*2000; // multiply by 2000 to have Lx
digitalWrite(LPPIN, LOW); // switch off the sensor
  return result;
}



int capteur_lumiere(){
 int lum = readLight();
 return lum;
}

int temp(){
  int temperature = sensor.getCelsiusHundredths()/100;
  return temperature;
  
}
int humi() {
  int humidity = sensor.getHumidityPercent();
  return humidity;
}

void loop() {
int payloadT = temp() ;
int payloadH = humi();
int payloadL = capteur_lumiere(); 

// send packet
  
  LoRa.beginPacket();
  LoRa.print("packet N°");
  LoRa.print(counter);
  LoRa.print(" ");
  LoRa.print(payloadT);
  LoRa.print("°C  ");  
  LoRa.print(payloadH);
  LoRa.print("%   ");
  LoRa.print(payloadL); 
  LoRa.print("Lx   ");
  LoRa.endPacket();
  counter++;

  Serial.print("Sending packet with payload {");
  //Serial.print(payloadT;payloadH,payloadL );
  Serial.print("} N°");
  Serial.println(counter);
 

  //delay(10);
  Serial.print("  temp : ");
  Serial.print(payloadT);
  Serial.print("°C"); 
  //delay(100);
  Serial.print("  humi : ");
  Serial.print(payloadH);
  Serial.print("%");
  //delay(100); 
  Serial.print("  lum : ");
  Serial.print(payloadL);
  Serial.println("lx");
  Serial.println();
  //delay(500);
  
  /* // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }*/
  
  //Serial.print(temp());
  
 
  
  //Serial.print(humi());
  
  
  
  //Serial.print(capteur_lumiere());
  
  
  
  // put your main code here, to run repeatedly:

}
