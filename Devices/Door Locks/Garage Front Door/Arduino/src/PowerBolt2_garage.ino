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
  Name:     PowerBolt2 Hack - GARAGE FD
  Created:  2019/07/10
  Modified: 2020/03/24
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/
*/

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                     7
//ADC_MODE(ADC_VCC);                              //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnet mask here

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
const uint8_t boardLedPin         = 2;            //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
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
// ---------------------------------------- POWERBOLT2 DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------
//When looking at the deadbolt installed on the door, looking from the INSIDE, does the lock is
//pointing toward the right? 
// If yes, set this tag to true, otherwise set it to false
bool LOCKED_ON_RIGHT              = false;

//When looking at the inside of the gearbox, does the top gear appears solid or not? 
//Does it have two or three limit switches?
// If it has solid gear with 3 limits switches, set this tag to true, otherwise set it to false
#define SOLID_GEAR                  true

const uint16_t maxTimeMotor       = 5000;
const uint16_t minRevMotor        = 500;

long runTimeMotor                 = 0;

long reverseTimeMotor             = 0;
long holdTimeMotor                = 0;

//Limit switches that determines the locked/unlocked status
bool limitSwitch1                 = false;        //Top (Negative side)
bool limitSwitch2                 = false;        //Top (Positive side)

const uint8_t limitSwitch1Pin     = 12;
const uint8_t limitSwitch2Pin     = 13;

//Limit switches that determines the master gear position - ONLY ON SOLID GEAR
// This gear has slots in many positions. The gear has to be middled all the time (default position)
// When locking or unlocking, you have to hold the motor until you reach the next slot. Limit switch 
// is off whenever it is in a slot, so you start the motor and once it goes high, you wait until it 
// goes low again
bool limitSwitch3                 = false;        //Bottom (Master gear position)
bool fallingEdgeLS3               = false;        //Used to detect falling edge on signal

const uint8_t limitSwitch3Pin     = 14;

//Door Commands/Status
bool lockRequest                  = false;        //Lock request received from MQTT
bool unlockRequest                = false;        //Unlock request received from MQTT
bool middleRequestLock            = false;        //Return to middle after lock
bool middleRequestUnlock          = false;        //Return to middle after unlock
bool newRequest                   = false;        //New request from server

bool locked                       = false;        //Locked Status Validated
bool unlocked                     = false;        //Unlocked Status Validated

bool lockStatus                   = false;        //Lock status to MQTT (true=locked)

//Door Motor outputs
// Activate only one at a time
const uint8_t Motor1OutputPin     = 4;            //Motor in first direction (HIGH=Active)
const uint8_t Motor2OutputPin     = 5;            //Motor in second direction (HIGH=Active)

bool Motor1Output                 = false;        //Motor output bit
bool Motor2Output                 = false;        //Motor output bit

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- DOOR STATUS DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t doorReedPin         = 16;           //Door reed switch status

bool doorStatus                   = false;        //Door status to MQTT (true=closed)

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- DOOR BELL DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t doorbellPin         = 0;            //Door reed switch status

bool doorbellStatus               = false;        //Door status to MQTT (true=pushed)

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- BATTERY STATUS DEFINES --------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t voltPin             = A0;           //Battery voltage pin

uint8_t batStatus                 = 0;            //Battery voltage reading

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
  
  initSensors();

  initWifi();
  initOTA();
  
  initMQTT();

  lastSecond = millis();
  lastMinute = millis();
  
  local_delay(50);                              //Wait for all data to be ready
  
  //Initialize data
  newStart = true;
  
  lockRequest = false;
  unlockRequest = false;
  middleRequestLock = false;
  middleRequestUnlock = false;
  newRequest = false;

  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();

  //Wait until door lock has initialized itself before trying to do anything
  // Once initialized, the bottom limit switch will return to LOW value
  if (powerUp())
    return;

  mqttPublish();                                //Publish MQTT data
  
  runEverySecond();

  runEveryMinute();

  newStart = false;                             //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
  runOTA();
  
  mqttActive = runMQTT();
  networkActive = checkNetwork();

  mqttKeepRunning();

  mqttPublish();                                //Run to check nothing has been received and needs to republish

  local_delay(10);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000)
    return;

  unsigned long loopTime = millis();

  updatePublish = true;
  mqttPublish();                                //Publish MQTT data

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
