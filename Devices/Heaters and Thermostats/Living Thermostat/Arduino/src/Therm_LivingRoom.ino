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
  Name:     Thermostat - Living Room
  Created:  2020/10/02
  Modified: 2020/10/13
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/
*/

const char* version               = "v:2.1.2";
const char* date                  = "2020/10/25";

#define ESP32                       32
//#define ESP8266                   7

#if (defined(ESP32) and defined(ESP8266))
  #error "Please select only ONE board type (ESP32 or ESP8266)"
#elif defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #error "Please select ESP32 or ESP8266 board type"
#endif

//ADC_MODE(ADC_VCC);                              //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnetmask here

bool networkActive                = false;        //WiFi connectivity status
int rssiPercent                   = 0;            //WiFi signal strength in percent
int rssi                          = 0;            //WiFi signal strength in dBm

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifdef ESP32
  hw_timer_t *watchdogTimer       = NULL;         //Watchdog implementation
  #define LWD_TIMEOUT               5000          //WDT Value (ms)

  #include <rom/rtc.h>
#elif ESP8266
  #include <Ticker.h>
  Ticker lwdTicker;
  
  #define LWD_TIMEOUT               5000          //WDT Value (ms)
  
  unsigned long lwdTime           = 0;
#endif
// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifdef ESP32
  #include <ESPmDNS.h>
#elif ESP8266
  #include <ESP8266mDNS.h>
#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

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
//Used Pins
const uint8_t boardLedPin         = 2;//LED_BUILTIN;  //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32

#ifdef ESP32
  const bool boardLedPinRevert    = false;        //If true, LED is on when output is low
  const bool enableBoardLED       = true;         //If true, LED will flash to indicate status
#else
  const bool boardLedPinRevert    = true;         //If true, LED is on when output is low
  const bool enableBoardLED       = false;        //If true, LED will flash to indicate status
#endif

//Variables
#define initValue                   -1            //Initialization value to insure values updates

bool newStart                     = false;        //New start detection

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);
long lastMsg                      = 0;
char msg[50];

bool updatePublish                = false;
bool mqttActive                   = false;

long lastSecond                   = 0;
long lastMinute                   = 0;

uint8_t keepAlive                 = MQTT_KEEPALIVE;
uint8_t lastKeepAlive             = MQTT_KEEPALIVE*2;//Insure it will detect a new connection

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- MQTT JSON DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(100);
const int BUFFER_ARRAY_SIZE       = 255;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

#define EEPROM_SIZE                 64

int address_PWH                   = 0;
int address_PWL                   = 1;
int address_LED                   = 2;
int address_LCK                   = 3;
int address_DIM                   = 4;
int address_BIP                   = 5;
int address_VOL                   = 6;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
//#define ENABLE_VOLT_ADC

const uint8_t voltage5V_pin       = A7;

float voltage5V                   = 0.0;

//R1  = 10k
//R2  = 15k
//Vr2 = 3.30V @ 5.50V
//Theoretical ratio = 0.6

float voltage5VRatio              = 0.99298;        //Voltage divider ratio [0 => 5.5]Vdc

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
#define ENABLE_DHT

#ifdef ENABLE_DHT
  #include <Wire.h>
  #include "DHT.h"
  
  //#define DHTTYPE                 DHT11           // DHT 11
  #define DHTTYPE                   DHT22           // DHT 22 (AM2302), AM2321
  //#define DHTTYPE                 DHT21           // DHT 21 (AM2301)

  #ifdef ESP32
    #define DHTPin 17
  #elif ESP8266
    #define DHTPin 3
  #endif

  DHT dht(DHTPin, DHTTYPE);                         //Create DHT object
#endif

float iRoom_temperature           = initValue;      //DHT temperature output to MQTT
float iRoom_humidity              = initValue;      //DHT humidity output to MQTT

//Prev temp to update screen
float iRoom_prevTemp              = iRoom_temperature+1.0;
float iRoom_prevHum               = iRoom_humidity+1.0;

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- LCD/TFT DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
//#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#include "usergraphics.h"

#include "config_TFT.h"

#define ENABLE_SPLASH
#ifdef ENABLE_SPLASH
  #include "splashscreen.h"
  bool screenOff                  = false;
#else
  bool screenOff                  = true;
  #warning "Splash screen disabled"
#endif

bool screenDim                    = false;

#ifdef ESP32
  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
#elif ESP8266
  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif

uint8_t ledDelayOff               = 30;             //Time in seconds
long ledMillis                    = 0;

uint16_t returnToMain             = 30000;          //Time in milliseconds
long returnMillis                 = 0;

#define DEFAULT_BRIGHTNESS          3              //50% default brightness
uint8_t screenDimValue            = DEFAULT_BRIGHTNESS;

int setOptionScreen               = 1;

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- TOUCHSCREEN DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------
XPT2046_Touchscreen touch(TOUCH_CS, TOUCH_IRQ);

//SPIClass SPI1(VSPI);

int X,Y;
uint8_t Thermostat_mode           = BOOT;

uint8_t PMode                     = INIT;           // Program mode
uint8_t PrevMode                  = INIT;           // Previous Program mode
bool touchPressed                 = false;
uint8_t Timer_Cleaning            = 0;

TS_Point p;

bool beepON                       = false;
#define DEFAULT_VOLUME              2               // 25% defualt volume
uint8_t beepVolume                = DEFAULT_VOLUME; 

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- CODELOCK DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#define DEFAULT_PASSCODE            42  // This is default passcode, you can change the passcode through MQTT JSON command "passcode"

String symbol[4][4] =
{
  { "7", "8", "9" },
  { "4", "5", "6" },
  { "1", "2", "3" },
  { "C", "0", "OK" }
};

uint16_t passcode                 = DEFAULT_PASSCODE; //You can change the passcode through MQTT JSON command "passcode" then stored in EEPROM

long Number;
char action;

bool logged                       = false;

uint8_t logDelayOff               = 30;             //Time in seconds
long logMillis                    = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- HVAC DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define FANMODES_DEF                1
#define FANMODES_MIN                0
#define FANMODES_MAX                4
const char *fanModes[]            = {
                                    "Auto",
                                    "Quiet",
                                    "Low",
                                    "Medium",
                                    "High"
                                    };

#define RUNMODES_DEF                0
#define RUNMODES_MIN                0
#define RUNMODES_MAX                5
//In order with home assistant
//Except auto that has been placed to the end and would need to be first if icon is used on this LCD
const char *runModes[]            = {
                                    "heat",
                                    "cool",
                                    "dry",
                                    "fan_only",
                                    "off",
                                    "auto"
                                    };

byte configStateOn                = false;
byte configFanMode                = 0;
byte configSwingMode              = false;
byte configRunMode                = 0;
byte configTempSetpoint           = DEF_TEMPERATURE;

byte bufferStateOn                = true;
byte bufferFanMode                = 0;
byte bufferSwingMode              = false;
byte bufferRunMode                = 0;
byte bufferTempSetpoint           = DEF_TEMPERATURE;

bool tempF                        = false;

bool stateHVAC                    = false;
bool notif                        = false;
bool eco                          = true;

float air_temp                    = initValue;    //Temperature coming out of HVAC fins

bool hvacAlive                    = false;

#define WAITBEFORESENDING
#define SENDDATADELAY               3000
long sendTempMillis               = 0;
bool sendTempWait                 = false;

long sendFanMillis                = 0;
bool sendFanWait                  = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>              // TimeLib library is needed https://github.com/PaulStoffregen/Time
                                  // http://playground.arduino.cc/code/time (Margolis 1.5.0)

#define DELIMITER_TIME              ":"

//Variables
uint16_t Year                     = initValue;
uint8_t Month                     = initValue;
uint8_t Day                       = initValue;
uint8_t Hour                      = initValue;
uint8_t Minute                    = initValue;
uint8_t Second                    = initValue;
uint8_t DOW                       = initValue;

String HourMinute                 = "  --:--";
String prevHourMinute             = "";

bool ONS_1s                       = false;
bool ONS_5s                       = false;
bool ONS_10s                      = false;
bool ONS_1m                       = false;

bool localTimeValid               = false;        //Detect that local time is valid

long lastTimeRequestMillis        = millis();

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial);
  #endif

  initWDT();
  checkResetCause();

  //Run early for SplashScreen
  initSensors();
  initTFT();

  initEEPROM();

  initWifi();
  initOTA();

  initMQTT();

  lastSecond = millis();
  lastMinute = millis();

  local_delay(50);

  //Initialize data
  newStart = true;
  
  //ledMillis = millis() + ledDelayOff*1000;
  returnMillis = millis() + returnToMain;

  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();

  runEverySecond();
  //runEvery5seconds();
  //runEvery10seconds();

  runEveryMinute();

  newStart = false;                                  //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
  runOTA();
  
  mqttActive = runMQTT();
  networkActive = checkNetwork();

  mqttKeepRunning();

  mqttPublish();                                     //Run to check nothing has been received and needs to republish
  
  local_delay(5);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000)
    return;

  unsigned long loopTime = millis();

  updatePublish = true;
  mqttPublish();                                     //Publish MQTT data

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
    
  Sprintln("5 seconds");

  flashEvery5sec();

  //screenDim = !screenDim;
  //screenDim = true;
  
  readSensors(true);
  local_delay(10);
  sendSensors();
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;

  Sprintln("10 seconds");
  local_delay(100);
  
  //TODO
}

void runEveryMinute()
{
  if (millis()-lastMinute < 60000)
    return;

  unsigned long loopTime = millis();

  //TODO

  lastMinute = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}
