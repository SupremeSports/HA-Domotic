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
 Name:    Pool Clock - Neo7Segment
 Created: 2019/01/02
 Author:  gauthier_j100@hotmail.com / SupremeSports
 GitHub:  https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Pool%20Clock
*/

/*
  ArduinoOTA            1.0.5
  ESP8266WiFi           
  PubSubClient          2.8.0
  ArduinoJson           5.13.5 (6.17.2)
  
  Board                 ESP8266-07 - ESP Board (2.7.1) - Generic ESP8266 Module
*/

const char* version               = "v:8.1.0";
const char* date                  = "2021/07/06";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                     7
ADC_MODE(ADC_VCC);                                  //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnet mask here
IPAddress dns(GW1, GW2, GW3, GW4);                //Put your DNS here
IPAddress dns1 = (uint32_t)0x00000000;
IPAddress dns2 = (uint32_t)0x00000000;

bool networkActive                = false;          //WiFi connectivity status
int rssiPercent                   = 0;              //WiFi signal strength in percent
int rssi                          = 0;              //WiFi signal strength in dBm

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Ticker.h>
Ticker lwdTicker;

#define LWD_TIMEOUT                 5000            //WDT Value (ms)

unsigned long lwdTime             = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define ENABLE_OTA

#ifdef ENABLE_OTA
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
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

#define BAUDRATE                    115200

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin         = 2;              //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = true;           //If true, LED is on when output is low
const bool enableBoardLED         = true;           //If true, LED will flash to indicate status

//Variables
#define initValue                   -99             //Initialization value to insure values updates

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
#include <math.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const uint16_t keepAlive          = 45;
const uint16_t packetSize         = 256;//1500;
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

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(100);//(200);
const int BUFFER_ARRAY_SIZE       = 255;//512;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

#define EEPROM_SIZE                 64

int address_Sts                   = 0;
int address_CAS                   = 1;
int address_CCr                   = 2;
int address_Scr                   = 3;
int address_Cyc                   = 4;
int address_Opt                   = 5;
int address_Lum                   = 6;
int address_Red                   = 7;
int address_Grn                   = 8;
int address_Blu                   = 9;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//const uint8_t sda                 = 0;            //I2C Data pin
//const uint8_t scl                 = 2;            //I2C Clock pin

// ---------------------------------- NEOPIXEL SEVEN SEGMENT DEFINES ----------------------------------

#include <Neo7Segment.h>

#define ENABLE_TEXTSTRINGS

//Used Pins
#define timePin                     14
#define tempPin                     12

//Displays setup
#define timeDigits                  6
#define timePixels                  8
#define timePixDp                   2
#define tempDigits                  8
#define tempPixels                  4
#define tempPixDp                   1

// Initalise the displays
Neo7Segment timeDisplay(timeDigits, timePixels, timePixDp, timePin);
Neo7Segment tempDisplay(tempDigits, tempPixels, tempPixDp, tempPin);

#define timeStandardColor           16711680          //= timeDisplay.Color(255, 0, 0);
#define tempStandardColor           65280             //= tempDisplay.Color(0, 255, 0);
#define waterStandardColor          255               //= waterDisplay.Color(0, 0, 255);

//All possible alphabet characters that can be displayed on a 7 segments
const String alphabet[26]         = {"A","B","C","D","E","F","G","H","I","J"," ","L"," ","N","O","P","Q","R","S","T","U"," "," ","X","Y","Z"};

bool changeColor                  = false;          //Change display color 
bool initDisplays                 = true;           //Initialize displays

#define EFFECT_DEFAULT              0   //Default
#define EFFECT_MIN                  0
#define EFFECT_MAX                  7

const char *effects[]             = {
                                    "Default",
                                    "RainbowCycle",
                                    "TextColor",
                                    "Spoon",
                                    "VertRainbowCycle",
                                    "HorizRainbowCycle",
                                    "TextChaser",
                                    "AllEffectsRolling"
                                    };

//Rainbow indexes and variables
unsigned long nextRainbow         = 0;
unsigned long nextSwitch          = 0;
unsigned long loopDelay           = 0;
byte rainbowIndex                 = 0;
byte displayFeature               = 96;             //Initialize displays, put to zero to disable
uint8_t loopIndex                 = 0;

// Maintained state for reporting to HA
byte stateOn                      = false;
byte configRedCnl                 = 255;
byte configGreenCnl               = 255;
byte configBlueCnl                = 255;
int configScrollSpeed             = 255;            //Text scrolling speed in ms (updates every 250 ms)
int configCycleSpeed              = 1;              //Text color cycling speed step/scan (increases by 1 at each scan)

#define defaultBrightness           80
byte brightness                   = defaultBrightness;

// ------------------------------------------ TIME DISPLAY --------------------------------------------
//Variables
uint8_t rollingTimeTextIndex      = 0;              //Scroll start index in string
long timeTextScrollLast           = 0;              //millis since the last scroll

String timeString;                                  //Time display string
String timeTextString;                              //Text display string
String timeTextStringBuffer;                        //Text display full string buffer

// ------------------------------------------ TEMP DISPLAY --------------------------------------------
//Variables
float outTempC                    = initValue;      //Input in °C
float outTempF                    = initValue;      //Input in °F
float outHumidity                 = initValue;      //Input in %
float waterTempF                  = initValue;      //Input in °F
float waterTempC                  = initValue;      //Input in °C
float waterLevelPH                = initValue;      //Input in pH level

uint8_t rollingTempTextIndex      = 0;              //Scroll start index in string
long tempTextScrollLast           = 0;              //millis since the last scroll

String tempString;                                  //Outside temperature display string
String waterString;                                 //Water temperature display string
String tempTextString;                              //Text display string
String tempTextStringBuffer;                        //Text display full string buffer

bool prevTempONS                  = false;          //One shot for text changing sequence
bool prevWaterONS                 = false;          //One shot for text changing sequence

// -------------------------------------- ANALOG SENSORS DEFINES --------------------------------------
//TODO

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t voltage5V_pin       = A0;
const uint8_t voltage12V_pin      = A0;

float voltage5V                   = 0.0;
float voltage12V                  = 0.0;

//R1  = 1k
//R2  = 10k
//Vr2 = 1.00V @ 5.50V
//Thorical ratio = 0.90909
float voltage5VRatio              = 1.0;      //Voltage divider ratio [0 => 5.5]Vdc

//R1  = 10k
//R2  = 5k6
//Vr2 = 1.00V @ 13.93V
//Thorical ratio = 2.78571
float voltage12VRatio             = 1.0;      //Voltage divider ratio [0 => 13.93]Vdc

// -------------------------------------- DIGITAL SENSORS DEFINES -------------------------------------
//TODO

// -------------------------------------- DIGITAL SENSORS DEFINES -------------------------------------
unsigned long forceTimeON         = 0;              //Timestamp of force ON request event
bool forceON                      = false;          //Display is forced ON no matter the time of day
bool forceOFF                     = false;          //Display is forced OFF no matter the time of day

unsigned long lastButtonPress     = 0;              //Timestamp of button pressed event

bool buttonActive                 = false;          //Button is currently pressed (used to one shot timer)
bool shortPressActive             = false;          //Button has reached short press delay
bool medPressActive               = false;          //Button has reached medium press delay
bool longPressActive              = false;          //Button has reached long press delay

bool colorAutoSwitch              = false;          //Displays are in auto color switch mode (scrolls between modes)

#define shortPressTime              250
#define medPressTime                2000
#define longPressTime               5000
#define extraTime                   3600000

#define buttonPin                   0

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

uint8_t prevHour                  = initValue;
uint8_t prevMinute                = initValue;
uint8_t prevSecond                = initValue;

bool localTimeValid               = false;          //Detect that local time is valid

long lastTimeRequestMillis        = millis();

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- SETUP ------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(BAUDRATE);
    while (!Serial);
  #endif

  initWDT();

  initSensors();
  initEEPROM();

  initWifi();
  initOTA();

  initDisplay();
  
  initMQTT();

  local_delay(5);                                //Wait for all data to be ready
  
  //Initialize data
  newStart = true;
  loopIndex = 0;

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

  newStart = false;                               //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
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

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  lastSecond = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");

  Sprint("State: ");
  Sprintln(stateOn ? mqtt_cmdOn : mqtt_cmdOff);
}

void runEvery5seconds()
{
  if (!ONS_5s)
    return;

  ONS_5s = false;
  PRE_5s = false;
    
  Sprintln("5 seconds");

  flashEvery5sec();
  
  readSensors(true);
  local_delay(10);
  //sendDigAnStates();
  sendSensors();
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;

  ONS_10s = false;
    
  Sprintln("10 seconds");
  local_delay(100);
  
  //TODO
}

void runEveryMinute()
{
  if (!ONS_1m)
    return;

  ONS_1m = false;

  unsigned long loopTime = millis();

  sendLightColorsState();
  sendCommandState();

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
