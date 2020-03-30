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
 Name:    HVAC Control - Living Room Fujitsu (ARRAH2E)
 Created: 2020/01/19
 Created: 2020/03/28
 Author:  gauthier_j100@hotmail.com / SupremeSports
 GitHub:  https://github.com/SupremeSports/
*/

// PROGRAMMING PORT
//J = TX
//I = RX
//H = GPIO0
//G = GROUND
//F = 3.3V 

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
#define DEBUG

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))
  #define Sprint(a) (Serial.print(a))
  #define Sprintf(a, b) (Serial.printf(a, b))
#else
  #define Sprintln(a)
  #define Sprint(a)
  #define Sprintf(a, b)
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
int address_fan                   = 1;
int address_swing                 = 2;
int address_run                   = 3;
int address_temp                  = 4;

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- HVAC FUJITSU IR CONTROL -------------------------------------
// ----------------------------------------------------------------------------------------------------
// Copyright 2017 Jonny Graham, 2018 David Conran
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Fujitsu.h>

#define MODEL                       ARRAH2E

const uint16_t kIrLed             = 4;            // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRFujitsuAC ac(kIrLed);

uint8_t defTemp                   = 21;
uint8_t minTemp                   = kFujitsuAcMinTemp; //16°C
uint8_t maxTemp                   = kFujitsuAcMaxTemp; //30°C

#define FANMODES_DEF                1
#define FANMODES_MIN                0
#define FANMODES_MAX                4
const char *fanModes[]            = {
                                    "Auto",
                                    "High",
                                    "Medium",
                                    "Low",
                                    "Quiet"
                                    };

#define RUNMODES_DEF                0
#define RUNMODES_MIN                0
#define RUNMODES_MAX                5
const char *runModes[]            = {
                                    "off",
                                    "auto",
                                    "cool",
                                    "dry",
                                    "fan_only",
                                    "heat"
                                    };

byte configStateOn                = false;
byte configFanMode                = 0; 
byte configSwingMode              = false;
byte configRunMode                = 0;
byte configTempSetpoint           = 21;

bool sendDataToHvac               = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- AIR TEMP STATUS DEFINES --------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t tempPin             = A0;           //Temperature voltage pin

float outputAirTemp               = 0;            //Temperature voltage reading

//Constants
float temp_ax2                    =  -0.00004;
float temp_bx                     =   0.09375;
float temp_c                      = -15.77583;

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

  initEEPROM();

  initMQTT();

  initFujitsuIR();

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

  local_delay(10);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000 && !sendDataToHvac)
    return;

  unsigned long loopTime = millis();

  if (sendDataToHvac)
    sendIRsignal();

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  sendDataToHvac = false;

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
