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
  Name:     Water Heater
  Created:  2020/03/29
  Modified: 2020/03/29
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/

  VERY IMPORTANT - 160MHz CPU Frequency
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
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

#define EEPROM_SIZE                 64

int address_state                 = 0;

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- WATER HEATER DEFINES ---------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t waterHeaterPin      = 15;           //Water Heater Activation

bool waterHeaterOn                = true;         //Water Heater ON (DEFAULT TO ON TO PREVENT DOWNTIME)

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- WATER STATUS DEFINES ---------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t tempPin             = A0;           //Temperature pin

const uint8_t tempInPin           = 12;           //Temperature input pin
const uint8_t tempOutPin          = 14;           //Temperature output pin
const uint8_t waterFlowPin        = 13;           //Water flow pin

float inputWaterTemp              = 0.0;          //Water Input Temperature reading
float outputWaterTemp             = 0.0;          //Water Output Temperature reading
float inputWaterFlow              = 0.0;          //Water flow reading

long waterFlowSecond              = 0;

uint16_t waterFlowBuffer          = 0;
byte waterFlowPrev                = 0;

//Constants - Input temp
float inTemp_ax2                  = -0.00005;
float inTemp_bx                   =  0.12001;
float inTemp_c                    = -8.14594;

//Constants - Output temp
float outTemp_ax2                 = -0.00005;
float outTemp_bx                  =  0.12001;
float outTemp_c                   = -8.14594;

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

  initEEPROM();                                 //Exceptionnaly declared BEFORE sensors

  initSensors();

  initWifi();
  initOTA();

  //initEEPROM();                               //Exceptionnaly declared BEFORE sensors
  
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

  newStart = false;                            //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
  runOTA();
  
  mqttActive = runMQTT();
  networkActive = checkNetwork();

  mqttKeepRunning();

  mqttPublish();                               //Run to check nothing has been received and needs to republish

  local_delay(2);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000)
    return;

  unsigned long loopTime = millis();

  updatePublish = true;
  mqttPublish();                               //Publish MQTT data

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
