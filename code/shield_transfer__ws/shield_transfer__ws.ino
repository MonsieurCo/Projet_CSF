
#include <Wire.h> //I2C needed for sensors
#include "SparkFunMPL3115A2.h" //Pressure sensor - Search "SparkFun MPL3115" and install from Library Manager
#include "SparkFun_Si7021_Breakout_Library.h" //Humidity sensor - Search "SparkFun Si7021" and install from Library Manager
MPL3115A2 myPressure; //Create an instance of the pressure sensor
Weather myHumidity;//Create an instance of the humidity sensor

//Hardware pin definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const byte STAT_BLUE = 7;
const byte STAT_GREEN = 8;

const byte REFERENCE_3V3 = A3;
const byte LIGHT = A1;
const byte BATT = A2;
const byte WSPEED = 3;

//Global Variables
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
long lastSecond; //The millis counter to see when a second rolls by
float windspeedkmh = 0;
int winddir_avg2m = 0;
float windgustkmh = 0;
int count = 1;
float acc = 0;
long lastWindCheck = 0;
volatile byte windClicks = 0;
volatile long lastWindIRQ = 0;

byte seconds; //When it hits 60, increase the current minute
byte seconds_2m; //Keeps track of the "wind speed/dir avg" over last 2 minutes array of data
byte minutes; //Keeps track of where we are in various arrays of data
byte minutes_10m; //Keeps track of where we are in wind gust/dir over last 10 minutes array of data
float windgust_10m[10];

#include <SoftwareSerial.h>
SoftwareSerial uno(10,11);// Rx Tx
String payload = "Envoi données: ";



void setup() {
  uno.begin(9600);
  Serial.begin(9600);

  pinMode(WSPEED, INPUT_PULLUP); // input from wind meters windspeed sensor
  
  
  pinMode(STAT_BLUE, OUTPUT); //Status LED Blue
  pinMode(STAT_GREEN, OUTPUT); //Status LED Green

  pinMode(REFERENCE_3V3, INPUT);
  pinMode(LIGHT, INPUT);

  attachInterrupt(1, wspeedIRQ, FALLING);

  interrupts();

  //Configure the pressure sensor
  myPressure.begin(); // Get sensor online
  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  //Configure the humidity sensor
  myHumidity.begin();

  lastSecond = millis();


}


float get_light_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float lightSensor = analogRead(LIGHT);

  operatingVoltage = 3.3 / operatingVoltage; //The reference voltage is 3.3V

  lightSensor = operatingVoltage * lightSensor;

  return (lightSensor);
}

//Returns the voltage of the raw pin based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
//Battery level is connected to the RAW pin on Arduino and is fed through two 5% resistors:
//3.9K on the high side (R1), and 1K on the low side (R2)
float get_battery_level()
{
  float operatingVoltage = analogRead(REFERENCE_3V3);

  float rawVoltage = analogRead(BATT);

  operatingVoltage = 3.30 / operatingVoltage; //The reference voltage is 3.3V

  rawVoltage = operatingVoltage * rawVoltage; //Convert the 0 to 1023 int to actual voltage on BATT pin

  rawVoltage *= 4.90; //(3.9k+1k)/1k - multiple BATT voltage by the voltage divider to get actual system voltage

  return (rawVoltage);
}

void wspeedIRQ()
// Activated by the magnet in the anemometer (2 ticks per rotation), attached to input D3
{
  if (millis() - lastWindIRQ > 10) // Ignore switch-bounce glitches less than 10ms (142MPH max reading) after the reed switch closes
  {
    lastWindIRQ = millis(); //Grab the current time
    windClicks++; //There is 1.492MPH for each click per second.
  }
}

float get_wind_speed()
{
  float deltaTime = millis() - lastWindCheck; //750ms

  deltaTime /= 1000.0; //Covert to seconds

  float windSpeed = (float)windClicks / deltaTime; //3 / 0.750s = 4

  windClicks = 0; //Reset and start watching for new wind
  lastWindCheck = millis();

  windSpeed *= 1.492; //4 * 1.492 = 5.968MPH

  return (windSpeed);
}




void loop() 
{
  acc =+ get_wind_speed();
  float av = acc / count;
  count ++;
  
 float  currentSpeed = get_wind_speed();
float speedw = currentSpeed;
// Serial.print(currentSpeed);
 //Serial.println(windgustkmh);

 
 
   if (speedw <= get_wind_speed())
    {
      Serial.print("hello world");
      windgustkmh = currentSpeed;

    }
       
   //Print readings every second
  if (millis() - lastSecond >= 1000)
  {
    digitalWrite(STAT_BLUE, HIGH); //Blink stat LED

    lastSecond += 1000;

    //Check Humidity Sensor
    float humidity = myHumidity.getRH();

    if (humidity == 998) //Humidty sensor failed to respond
    {
      Serial.println("I2C communication to sensors is not working. Check solder connections.");

      //Try re-initializing the I2C comm and the sensors
      myPressure.begin();
      myPressure.setModeBarometer();
      myPressure.setOversampleRate(7);
      myPressure.enableEventFlags();
      myHumidity.begin();
    }
    else
    {
      payload += "H= ";
      payload += humidity;
      payload +="%,";
      float temp_h = myHumidity.getTemp();
      String temp_h2 ="";
      temp_h2+= temp_h;
      payload += "t= ";
      payload += temp_h2;
      payload += "°C,";

      //Check Pressure Sensor
      float pressure = myPressure.readPressure();
      String pressure2 ="";
      pressure2 += pressure;
      
      payload += " P= ";
      payload += pressure2;
      payload += " Pa,";

      //Check tempf from pressure sensor
     // float tempf = myPressure.readTemp();
     // payload += " temp_p = ";
     // payload += tempf;
     // payload += "°C,";

      //Check light sensor
      float light_lvl = get_light_level();
      String light = "";
      light += light_lvl;
      payload +=" l= ";
     payload += light;
      payload += "V,";


      float windspeedkmh = get_wind_speed()*1.605;
      String WS = "";
      WS += windspeedkmh;
      payload +=" V= ";
     payload += WS;
      payload += "Km/h";


      float windgust = windgustkmh;
      String wmx = "";
      wmx += windgustkmh;
      payload +=" Vmax= ";
     payload += wmx;
      payload += "Km/h";

      
      String M = "";
      M += av;
      payload +=" Vmoy= ";
     payload += M;
      payload += "Km/h";

      //Check batt level
      float batt_lvl = get_battery_level();
      String batt="";
      batt += batt_lvl;
      payload += " Vin = ";
      payload += batt;
      payload +="V";

      

      

      

      Serial.println(payload);
      Serial.println(payload.length());
      
      
    }

    digitalWrite(STAT_BLUE, LOW); //Turn off stat LED
  }

  delay(100);
    for (int i = 0;i<payload.length(); i++)
   {
    uno.write(payload.charAt(i));
    delay(50);
    }
    payload = "Envoi données: ";
    
  
  delay(500);
}

//Returns the voltage of the light sensor based on the 3.3V rail
//This allows us to ignore what VCC might be (an Arduino plugged into USB has VCC of 4.5 to 5.2V)
