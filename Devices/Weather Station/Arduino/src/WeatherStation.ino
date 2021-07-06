/*
Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met :

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and / or other materials
provided with the distribution.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  The views and conclusions contained in the software and documentation are those of the
  authors and should not be interpreted as representing official policies, either expressed
  or implied
*/

/*
  Name:   Weather Station
  Created: 2019/09/29
  Author:  gauthier_j100@hotmail.com / SupremeSports
  GitHub:  https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Weather%20Station
*/

/*
  ArduinoOTA            1.0.5
  UIPEthernet           2.0.9
  PubSubClient          2.8.0
  ArduinoJson           6.17.2
  Wire 
  DHT sensor library    1.4.1
  Board                 Arduino Mega 2560 (Optiboot)
*/

const char* version               = "v:2.2.4";
const char* date                  = "2021/03/25";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- Ethernet DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <UIPEthernet.h>

#define MEGA2560                    1
#define ENC28J60                    1

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

byte mac[]                        = {MC0,MC1,MC2,MC3,MC4,MC5};
IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gateway(GW1, GW2, GW3, GW4);            //Put your gateway IP address here
IPAddress subnet(SN1, SN2, SN3, SN4);             //Put your subnet mask here
IPAddress dnsIp(IP1, IP2, IP3, IP4);              //Put your DNS Server here

#define ENC28J60_SS_pin             53

bool networkActive                = false;        //WiFi connectivity status

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <avr/wdt.h>

const bool ENABLE_WDT             = true;

const uint8_t selfReset_pin       = 7;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define ENABLE_OTA

#ifdef ENABLE_OTA
  #define NO_OTA_PORT
  #include <ArduinoOTA.h>
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DEBUG DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//#define DEBUG

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))
  #define Sprint(a) (Serial.print(a))
#else
  #define Sprintln(a)
  #define Sprint(a)
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t boardLedPin         = LED_BUILTIN;  //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   -99           //Initialization value to insure values updates

long lastSecond                   = 0;

bool newStart                     = false;        //New start detection
bool ONS_1s                       = false;
bool ONS_5s                       = false;
bool ONS_10s                      = false;
bool ONS_1m                       = false;

bool PRE_5s                       = false;        //Comes on for the second before ONS_5s comes on

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const uint16_t keepAlive          = 45;
const uint16_t packetSize         = 1500;
const uint16_t socketTimeout      = 60;

long lastMsg                      = 0;
char msg[50];

bool updatePublish                = false;
bool mqttActive                   = false;

bool forceMqttUpdate              = false;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- MQTT JSON DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(200);
const int BUFFER_ARRAY_SIZE       = 512;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t sda                 = 20;           //I2C Data pin
const uint8_t scl                 = 21;           //I2C Clock pin

// -------------------------------------- WIND/RAIN I2C DEFINES ---------------------------------------
//#include <Wire.h>
#ifndef I2CDefs_h
  #define I2CDefs_h
  #define I2C_SLAVE_ADDR            0x69
  #define I2C_DATA_SIZE             6
  #define I2C_PROTOCOL_NUMBER       54
#endif

// ---------------------------------------- WIND SPEED DEFINES ----------------------------------------
#define I2C_WINDSPD

#ifdef I2C_WINDSPD
  float windSpdOut                = 0;             //Wind speed frequency output to MQTT
#else
  const uint8_t windSpdPin        = 49;            //Wind speed sensor (pin 49 for FreqMeasure / 47 for FreqCount)
  
  long lastWindSpeed              = 0;             //Delay between readings
  
  uint16_t windSpeedBuffer        = 0;             //Wind pulse counter
  byte windSpeedPrev              = 0;             //Last status of wind sensor

  uint16_t windSpdOut             = 0;             //Wind speed frequency output to MQTT
#endif



// -------------------------------------- WIND DIRECTION DEFINES --------------------------------------
#define WindDirOffset               1              //Offset angle in degrees

const uint8_t windDir1Pin         = 38;            //Wind direction sensor #1
const uint8_t windDir2Pin         = 40;            //Wind direction sensor #2
const uint8_t windDir3Pin         = 44;            //Wind direction sensor #3
const uint8_t windDir4Pin         = 42;            //Wind direction sensor #4
const uint8_t windDir5Pin         = 46;            //Wind direction sensor #5

int16_t windDirOut                = initValue;     //Wind speed frequency output to MQTT

// --------------------------------------- RAIN SENSORS DEFINES ---------------------------------------
const uint8_t rainSensorPin       = A0;            //Rain sensor

int16_t RainSnsrOut               = initValue;     //Rain sensor output to MQTT

// ---------------------------------------- RAIN LEVEL DEFINES ----------------------------------------
#define I2C_RAINLVL

#ifdef I2C_RAINLVL
  //TODO
#else
  const uint8_t rainLevelPin      = 43;            //Rain level sensor
  unsigned long lastRainLvl       = 0;             //Delay between readings
  
  int16_t rainLvlBuffer           = 0;             //Rain level counter
  byte rainLvlPrev                = 0;             //Last status of rain bucket
#endif

int16_t RainLvlOut                = 0;             //Rain level output to MQTT

// ---------------------------------------- NTC SENSOR DEFINES ----------------------------------------
#define NTC_EN

#ifdef NTC_EN
  const uint8_t ntcSensorPin      = A8;            //NTC sensor
#endif

int16_t NTCTempOut                = initValue;     //NTC temperature output to MQTT

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
//#include <Wire.h>
#include "DHT.h"

//#define DHTTYPE                   DHT11          // DHT 11
#define DHTTYPE                     DHT22          // DHT 22 (AM2302), AM2321
//#define DHTTYPE                   DHT21          // DHT 21 (AM2301)

#define EXTERNAL_EN

#ifdef EXTERNAL_EN
  const uint8_t DHTPin            = 48;            //DHT Temp/Hum sensor (1-wire)

  DHT dht(DHTPin, DHTTYPE);                        //Create DHT object
#endif

float DHTTempOut                  = initValue;     //DHT temperature output to MQTT
float DHTHumOut                   = initValue;     //DHT humidity output to MQTT

// ------------------------------------ DHT INTERNAL SENSOR DEFINES -----------------------------------
#define INTERNAL_EN

#ifdef INTERNAL_EN
  const uint8_t DHTInternalPin    = 41;            //DHT Temp/Hum sensor (1-wire)
  
  DHT dhtInternal(DHTInternalPin, DHTTYPE);        //Create DHT object
#endif

float DHTTempIn                   = initValue;     //DHT temperature output to MQTT
float DHTHumIn                    = initValue;     //DHT humidity output to MQTT

// ---------------------------------------- BME SENSOR DEFINES ----------------------------------------
//#define BME_EN

#ifdef BME_EN
  //#include <Wire.h>
  //#include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
  
  #define I2CAddress_BME            0x76
  
  Adafruit_BME280 bme;                             //Create BME280 object 
#endif

float BMETempOut                  = initValue;     //BME temperature output to MQTT
float BMEHumOut                   = initValue;     //BME humidity output to MQTT
float BMEBaroOut                  = initValue;     //BME barometric output to MQTT

bool BMEreadFailed                = false;

// ------------------------------------- VEML6070 SENSOR DEFINES --------------------------------------
#define UV_EN

#ifdef UV_EN
  //#include <Wire.h>
  #include "Adafruit_VEML6070.h"
  
  #define VEMLTIMECONST             VEML6070_4_T
  
  Adafruit_VEML6070 uv            = Adafruit_VEML6070();//Create VEML6070 object 
#endif

int16_t VEMLUVOut                 = initValue;     //VEML6070 barometric output to MQTT

// --------------------------------------- MQ135 SENSOR DEFINES ---------------------------------------
//#define MQ135_EN
const uint8_t MQ135Pin            = A7;            //MQ135 sensor (1-wire)

int16_t MQ135Out                  = initValue;     //MQ135 output to MQTT

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t voltage5V_pin       = A2;
const uint8_t voltage12V_pin      = A1;

float voltage12V                  = 0.0;           //Power supply voltage output to MQTT
float voltage5V                   = 0.0;           //5V dc rail voltage output to MQTT

//R1  = 1k
//R2  = 10k
//Vr2 = 1.00V @ 5.50V
//Thorical ratio = 0.90909
float voltage5VRatio              = 1.36;      //Voltage divider ratio [0 => 5.5]Vdc

//R1  = 10k
//R2  = 5k6
//Vr2 = 1.00V @ 13.93V
//Thorical ratio = 2.78571
float voltage12VRatio             = 2.5;      //Voltage divider ratio [0 => 15]Vdc

// -------------------------------------- DIGITAL SENSORS DEFINES -------------------------------------
//TODO

// ---------------------------------------- SELF-RESET DEFINES ----------------------------------------
unsigned long resetDelay          = 0;
bool resetONS                     = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>              // TimeLib library is needed https://github.com/PaulStoffregen/Time
                                  // http://playground.arduino.cc/code/time (Margolis 1.5.0)

//Variables
uint8_t Second                    = initValue;
uint8_t Minute                    = initValue;

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- SETUP ------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial);
  #endif

  initWDT();

  initSensors();

  initEthernet();
  initOTA();
  
  initMQTT();

  lastSecond = millis();

  local_delay(50);                              //Wait for all data to be ready

  //Initialize data
  newStart = true;

  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();
  
  runEverySecond();

  runEveryMinute();

  newStart = false;                                //First scan bit
}

void runEveryScan()
{ 
  wdt_reset();

  runOTA();

  mqttActive = runMQTT();
  networkActive = checkNetwork();

  mqttKeepRunning();

  mqttPublish();                                  //Run to check nothing has been received and needs to republish

  local_delay(5);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000)
    return;  

  unsigned long loopTime = millis();

  readSensors(newStart ? 5 : 1);                  //Read sensors (buttons, etc.)

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  lastSecond = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}

void runEvery5seconds()
{
  if (!ONS_5s)
    return;
  
  ONS_5s = false;
  PRE_5s = false;
    
  Sprintln("5 seconds");

  flashEvery5sec();

  readSensors(5);
  local_delay(10);
  sendSensors();
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;
    
  ONS_10s = false;
    
  Sprintln("10 seconds");

  //TODO
}

void runEveryMinute()
{
  if (!ONS_1m)
    return;

  ONS_1m = false;

  unsigned long loopTime = millis();

  //Todo

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}

void forceUpdateMQTT(bool force)
{
  if ((!forceMqttUpdate && !force) || ONS_5s) 
    return;

  //Do not send data if it is to be sent within the next second
  if (forceMqttUpdate && PRE_5s)
  {
    forceMqttUpdate = false;
    return;
  }

  forceMqttUpdate = false;

  //TODO
}
