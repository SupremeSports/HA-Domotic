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
  Name:   Weather Station (MQTT)
  Created: 2019/09/29
  Modified: 2020/03/25
  Author:  gauthier_j100@hotmail.com / SupremeSports
  GitHub:  https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Weather%20Station
*/

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
// ------------------------------------------ DEBUG DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define DEBUG

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))
  #define Sprint(a) (Serial.print(a))
#else
  #define Sprintln(a)
  #define Sprint(a)
#endif

#ifndef MEGA2560
  #define MEGA2560
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <avr/wdt.h>

const uint8_t resetPin            = 7;

const bool ENABLE_WDT             = true;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t boardLedPin         = LED_BUILTIN;  //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   -99           //Initialization value to insure values updates

bool newStart                     = false;        //New start detection

long ledFlashDelay                = 0;            //Led flashing delay
long lastSecond                   = 0;
long lastMinute                   = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
long lastMsg                      = 0;
char msg[50];

bool updatePublish                = false;
bool mqttActive                   = false;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- MQTT JSON DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(100);
const int BUFFER_ARRAY_SIZE       = 255;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- WEATHER DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------

// --------------------------------------- I2C SENSORS DEFINES ----------------------------------------
const uint8_t sda             = 20;             //I2C Data pin
const uint8_t scl             = 21;             //I2C Clock pin

// ---------------------------------------- WIND SPEED DEFINES ----------------------------------------
//#define FREQLIBRARIES

#ifdef FREQLIBRARIES
  #include <FreqCount.h>                           //Best for 1 kHz to 8 MHz
  #include <FreqMeasure.h>                         //Best for 0.1 Hz to 1 kHz
#endif

const uint8_t windSpdPin      = 49;                //Wind speed sensor (pin 49 for FreqMeasure / 47 for FreqCount)
float windSpdOut              = initValue;         //Wind speed frequency output to MQTT

long lastWindSpeed            = 0;

uint16_t windSpeedBuffer      = 0;
byte windSpeedPrev            = 0;

// -------------------------------------- WIND DIRECTION DEFINES --------------------------------------
#define WindDirOffset           1                  //Offset angle in degrees

const uint8_t windDir1Pin     = 38;                //Wind direction sensor #1
const uint8_t windDir2Pin     = 40;                //Wind direction sensor #2
const uint8_t windDir3Pin     = 44;                //Wind direction sensor #3
const uint8_t windDir4Pin     = 42;                //Wind direction sensor #4
const uint8_t windDir5Pin     = 46;                //Wind direction sensor #5

int16_t windDirOut            = initValue;         //Wind speed frequency output to MQTT

// --------------------------------------- RAIN SENSORS DEFINES ---------------------------------------
const uint8_t rainSensorPin   = A0;                //Rain sensor
const uint8_t rainLevelPin    = 43;                //Rain level sensor

int16_t rainLvlCounter        = 0;                 //Rain level counter
bool rainLvlMemory            = 0;                 //Last value of rain bucket
bool RainLvlStts              = false;             //Rain level sensor on/off

unsigned long rainLvlMillis   = 0;                 //Delay between readings

int16_t RainSnsrOut           = initValue;         //Rain sensor output to MQTT
int16_t RainLvlOut            = initValue;         //Rain sensor output to MQTT

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
//#include <Wire.h>
#include "DHT.h"

//#define DHTTYPE               DHT11              // DHT 11
#define DHTTYPE                 DHT22              // DHT 22 (AM2302), AM2321
//#define DHTTYPE               DHT21              // DHT 21 (AM2301)

const uint8_t DHTPin          = 48;                //DHT Temp/Hum sensor (1-wire)

float DHTTempOut              = initValue;         //DHT temperature output to MQTT
float DHTHumOut               = initValue;         //DHT humidity output to MQTT

DHT dhtExternal(DHTPin, DHTTYPE);                          //Create DHT object

// ------------------------------------ DHT INTERNAL SENSOR DEFINES -----------------------------------
//#include <Wire.h>
const uint8_t DHTInternalPin  = 41;                //DHT Temp/Hum sensor (1-wire)

float DHTTempIn               = initValue;         //DHT temperature output to MQTT
float DHTHumIn                = initValue;         //DHT humidity output to MQTT

DHT dhtInternal(DHTInternalPin, DHTTYPE);          //Create DHT object

// ---------------------------------------- BME SENSOR DEFINES ----------------------------------------
//#include <Wire.h>
//#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define I2CAddress_BME          0x76

float BMETempOut              = initValue;         //BME temperature output to MQTT
float BMEHumOut               = initValue;         //BME humidity output to MQTT
float BMEBaroOut              = initValue;         //BME barometric output to MQTT

bool BMEreadFailed            = false;

Adafruit_BME280 bme;                               //Create BME280 object 

// ------------------------------------- VEML6070 SENSOR DEFINES --------------------------------------
//#include <Wire.h>
#include "Adafruit_VEML6070.h"

#define VEMLTIMECONST           VEML6070_4_T

int16_t VEMLUVOut             = initValue;         //VEML6070 barometric output to MQTT

Adafruit_VEML6070 uv = Adafruit_VEML6070();        //Create VEML6070 object 

// --------------------------------------- MQ135 SENSOR DEFINES ---------------------------------------
const uint8_t MQ135Pin        = A7;                //MQ135 sensor (1-wire)

int16_t MQ135Out              = initValue;         //MQ135 output to MQTT

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t inputVoltPin    = A1;                //Power supply voltage
const uint8_t dcVoltPin       = A2;                //5V dc rail voltage

float inputVoltOut            = 0.0;               //Power supply voltage output to MQTT
float dcVoltOut               = 0.0;               //5V dc rail voltage output to MQTT

float inputVoltRatio          = 2.5;               //Voltage divider ratio [0 => 15]Vdc
float dcVoltRatio             = 1.36;              //Voltage divider ratio [0 => 5.5]Vdc

// ---------------------------------------- SELF-RESET DEFINES ----------------------------------------
const uint8_t selfResetPin    = 7;                 //MQ135 sensor (analog)

unsigned long resetDelay      = 0;
bool resetONS                 = false;
bool forceResetCmd            = false;

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
  initMQTT();

  lastSecond = millis();
  lastMinute = millis();

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

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  lastSecond = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}

void runEveryMinute()
{
  if (millis()-lastMinute < 60000)
    return;

  unsigned long loopTime = millis();

  //Todo

  lastMinute = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}
