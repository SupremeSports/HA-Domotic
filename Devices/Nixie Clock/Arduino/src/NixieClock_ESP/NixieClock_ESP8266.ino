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
  Name:     Nixie Clock - Time provider
  Created:  2019/01/02
  Modified: 2020/04/02
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Nixie%20Clock/Arduino/src/NixieClock_ESP

 All-In-One Rev 3 - WiFi Time Provider
  Firmware V1 Wifi
  
  ESP8266-01 Modbus Slave for the Arduino Nixie Clock Firmware V1
  Main code from: https://nixieclock.biz
  Reworked by: https://github.com/SupremeSports/

  Program with following settings (status line / IDE):
    Board: Generic ESP8266 Module, 
    Crystal Frequency: 26MHz,
    Flash: 40MHz, 
    CPU: 80MHz, 
    Flash Mode: DOUT, 
    Flash size: 1M (OTA: 470k), 
    Builtin LED: 1

    Need the Esp.zip patch from here for some ESP-01 modules:
    https://github.com/esp8266/Arduino/issues/4061
*/

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                     1
ADC_MODE(ADC_VCC);                                //Read Vcc on ADC input

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
#include <Ticker.h>
Ticker lwdTicker;

#define LWD_TIMEOUT                 5000          //WDT Value (ms)

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
#ifndef DEBUG //ESP01 uses pin #1 for LED and Tx, so disable LED when in debug mode
  const uint8_t boardLedPin       = 1;            //Pin 1 on ESP-01, pin 2 on ESP-07/12 and ESP32
#else
  const uint8_t boardLedPin       = -1;           //Disable pin that is also used for LED
#endif

const bool boardLedPinRevert      = true;         //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   -1            //Initialization value to insure values updates

bool newStart                     = false;        //New start detection

long ledFlashDelay                = 0;            //Led flashing delay

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
// --------------------------------------------- LED DATA ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Options “0”, “1” and “2”, do not dim with the bulbs. Options “3”, “4” and “5” do dim
const char *effects[]             = {
                                    "Fixed",
                                    "Pulse",
                                    "Cycle",
                                    "FixedDim",
                                    "PulseDim",
                                    "CycleDim"
                                    };

byte configBacklightOn;
byte configBacklightMode;
byte configRedCnl;
byte configGreenCnl;
byte configBlueCnl;
byte configCycleSpeed;
           
// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- NIXIE CLOCK -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Wire.h>

#include "I2CDefs.h"                              //Other parts of the code, broken out for clarity

#define I2C_SDA                     0
#define I2C_SCL                     2

byte preferredI2CSlaveAddress     = 0x69;
byte preferredAddressFoundBy      = 0;            // 0 = not found, 1 = found by default, 2 = found by ping

// Clock config
bool configStateOn;
byte configHourMode;
byte configBlankLead;
byte configScrollback;
byte configSuppressACP;
byte configUseFade;
byte configUseLDR;
byte configSlotsMode;
byte configDateFormat;
byte configDayBlanking;
byte configBlankFrom;
byte configBlankTo;
byte configFadeSteps;
byte configScrollSteps;
byte configBlankMode;
uint16_t configMinDim;

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

bool localTimeValid               = false;        //Detect that local time is valid

long lastTimeRequestMillis        = millis();

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- CASE TEMP DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
//LM75 used without library

//#define VERSION "1.1"
//#include <lm75.h>
//#include <Wire.h> //Already declared

byte preferredI2CTempAddress      = 0x48;

//LM75 Registers
byte temp_reg                     = 0x00;
byte config_reg                   = 0x01;
byte THyst_reg                    = 0x02;
byte TOS_reg                      = 0x03;

float caseTemp                    = 0.0;

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

  initWifi();
  initOTA();

  initMQTT();

  initI2C();
  initNixie();

  lastSecond = millis();
  lastMinute = millis();

  local_delay(50);                                //Wait for all data to be ready

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
}

void runEveryMinute()
{
//  if (millis()-lastMinute < 60000)
//    return;
  if ((Minute == prevMinute) || !localTimeValid)
    return;

  unsigned long loopTime = millis();

  if (getClockOptionsFromI2C())
  {
    sendLightColorsState();
    sendNixieStates();
  }

  prevMinute = Minute;
  lastMinute = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}
