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
 Created: 2020/04/17
 Author:  gauthier_j100@hotmail.com / SupremeSports
 GitHub:  https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Pool%20Clock
*/

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                     7
ADC_MODE(ADC_VCC);                                  //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                   //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                   //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                   //Put your subnetmask here

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
#include <ESP8266mDNS.h>
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

#ifndef ESP8266
  #define ESP8266
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin         = 2;//LED_BUILTIN;//Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = true;           //If true, LED is on when output is low
const bool enableBoardLED         = true;           //If true, LED will flash to indicate status

//Variables
#define initValue                   -99             //Initialization value to insure values updates

bool newStart                     = false;          //New start detection

long ledFlashDelay                = 0;              //Led flashing delay

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

int address_Sts                   = 0;
int address_CAS                   = 1;
int address_CCr                   = 2;
int address_Spd                   = 3;
int address_Opt                   = 4;
int address_Lum                   = 5;
int address_Red                   = 6;
int address_Grn                   = 7;
int address_Blu                   = 8;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------- NEOPIXEL SEVEN SEGMENT DEFINES ----------------------------------
// ----------------------------------------------------------------------------------------------------
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
                                    "AllEffectsRolling",
                                    };

//Rainbow indexes and variables
unsigned long nextRainbow         = 0;
unsigned long nextSwitch          = 0;
byte rainbowIndex                 = 0;
byte displayFeature               = 96;             //Initialize displays
uint8_t loopIndex                 = 0;

// Maintained state for reporting to HA
byte stateOn                      = false;
byte configRedCnl                 = 255;
byte configGreenCnl               = 255;
byte configBlueCnl                = 255;
int configCycleSpeed              = 250;            //Text scrolling speed in ms (updates every 250 ms)

#define defaultBrightness           20
byte brightness                   = 50;//defaultBrightness*255/100;

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

uint8_t rollingTimeTextIndex      = 0;              //Scroll start index in string
long timeTextScrollLast           = 0;              //millis since the last scroll

String timeString;                                  //Time display string
String timeTextString;                              //Text display string
String timeTextStringBuffer;                        //Text display full string buffer

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TEMP DISPLAY --------------------------------------------
// ----------------------------------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- BUTTONS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
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
  initEEPROM();

  initWifi();
  initOTA();

  initDisplay();
  
  initMQTT();
  
  lastSecond = millis();
  lastMinute = millis();

  local_delay(5);                                //Wait for all data to be ready
  
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

void runEveryMinute()
{
  if (millis()-lastMinute < 60000)
    return;

  unsigned long loopTime = millis();

  sendLightColorsState();

  lastMinute = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}
