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
  Name:     Dual Stepper Wall Clock
  Created:  2019/04/12
  Modified: 2020/03/10
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:  
*/

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                 1
ADC_MODE(ADC_VCC);                                //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnetmask here

bool networkActive              = false;          //WiFi connectivity status
int rssiPercent                 = 0;              //WiFi signal strength in percent
int rssi                        = 0;              //WiFi signal strength in dBm

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Ticker.h>
Ticker lwdTicker;

#define LWD_TIMEOUT               5000            //WDT Value (ms)

unsigned long lwdTime           = 0;

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
const uint8_t boardLedPin       = 2;//LED_BUILTIN;//Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert    = true;           //If true, LED is on when output is low
const bool enableBoardLED       = true;           //If true, LED will flash to indicate status

//Variables
#define initValue               -1                //Initialization value to insure values updates

bool newStart                   = false;          //New start detection

long ledFlashDelay              = 0;              //Led flashing delay

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);
long lastMsg                    = 0;
char msg[50];

bool updatePublish              = false;
bool mqttActive                 = false;

long lastMillis                 = 0;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- MQTT JSON DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ArduinoJson.h>        // https://github.com/bblanchon/ArduinoJson

const int BUFFER_SIZE           = JSON_OBJECT_SIZE(100);

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- CLOCK DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Define installed steppers and comment out the other ones
#define HOURS
#define MINS
//#define SECS

//Used Pins
#define ResetHoming               0

#define StepEnable                15

#define HourHall                  16
#define MinHall                   14
#define SecHall                   initValue

#define HourPulse                 12
#define MinPulse                  13
#define SecPulse                  initValue

#define Spare1                    initValue//5
#define Spare2                    initValue//4

//Other defines
#define PULSE_PER_REV             720
#define STEPPER_HOUR              0               //Motor ID
#define STEPPER_MIN               1               //Motor ID
#define STEPPER_SEC               2               //Motor ID
#define STEPPER_ALL               999

#define STEPPER_SPEED             2000            //us between HIGH/LOW

//Variables
int HourTick                    = initValue;
int MinTick                     = initValue;
int SecTick                     = initValue;

bool clockHomed                 = false;          // Homing status

bool initialHourOns             = false;
bool initialMinOns              = false;
bool initialSecOns              = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>            //TimeLib library is needed https://github.com/PaulStoffregen/Time

#define DELIMITER_TIME            ":"

//Variables
uint16_t Year                   = initValue;
uint8_t Month                   = initValue;
uint8_t Day                     = initValue;
uint8_t Hour                    = initValue;
uint8_t Minute                  = initValue;
uint8_t Second                  = initValue;
uint8_t DOW                     = initValue;

uint8_t prevSec                 = initValue;

bool localTimeValid             = false;

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

  lastMillis = millis();

  local_delay(50);                                //Wait for all data to be ready
  
  //Initialize data
  clockHomed = false;
  initialHourOns = false;
  initialMinOns = false;
  initialSecOns = false;
  
  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();

  runEverySecond();

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

  local_delay(10);
}

void runEverySecond()
{
  if (millis()-lastMillis < 1000)
    return;

  unsigned long loopTime = millis();

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  lastMillis = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}
