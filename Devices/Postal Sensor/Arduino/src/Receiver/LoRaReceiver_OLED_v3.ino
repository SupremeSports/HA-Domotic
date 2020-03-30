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
  Name:     LoRa Receiver with OLED display - Postal
  Created:  2019/06/05
  Modified: 2019/11/16
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:  
*/

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <WiFi.h>

#define ESP32_TTGO             1
//ADC_MODE(ADC_VCC);                              //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnetmask here

bool networkActive              = false;          //WiFi connectivity status

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
hw_timer_t *watchdogTimer       = NULL;           //Watchdog implementation

#define LWD_TIMEOUT               5000            //WDT Value (ms)

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESPmDNS.h>
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

#ifndef ESP32_TTGO
  #define ESP32_TTGO
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin       = LED_BUILTIN;    //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert    = false;          //If true, LED is on when output is low
const bool enableBoardLED       = true;           //If true, LED will flash to indicate status

//Variables
#define initValue                 -99             //Initialization value to insure values updates

bool newStart                   = false;          //New start detection

long ledFlashDelay              = 0;              //Led flashing delay

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>
#include <ArduinoJson.h>        // https://github.com/bblanchon/ArduinoJson
const int BUFFER_SIZE           = JSON_OBJECT_SIZE(40);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

long lastMsg                    = 0;
char msg[50];

bool updatePublish              = false;
bool mqttActive                 = false;

long lastMillis                 = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

#define EEPROM_SIZE               64

int address_IDH                 = 0;
int address_IDL                 = 1;
int address_Stts                = 2;
int address_VccH                = 3;
int address_VccL                = 4;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- LoRa DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  

#define DELIMITER_LORA            ":"
#define NEWMAIL                   "NEW MAIL"
#define NOMAIL                    "NO MAIL"

#define SCK                       5    // GPIO5  -- SX1278's SCK
#define MISO                      19   // GPIO19 -- SX1278's MISO
#define MOSI                      27   // GPIO27 -- SX1278's MOSI
#define SS                        18   // GPIO18 -- SX1278's CS
#define RST                       14   // GPIO14 -- SX1278's RESET
#define DI0                       26   // GPIO26 -- SX1278's IRQ(Interrupt Request)
#define BAND                      915E6

String rssi                     = "RSSI --";
String rssiValue                = "";
String packSize                 = "--";
String packet                   = "";
String packetID                 = "0";

bool mailPresent                = false;
String Id                       = "0";
String Vcc                      = "0.00";
String Bat                      = "0";

int encryptKey                  = 200;

long delayMailPresent           = 0;
const long mailPresentOff       = 300000;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OLED DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include "SSD1306.h"
//#include <Wire.h>  //Already included for I2C comm to LoRa

#define SDA                       4
#define SCL                       15

SSD1306 display(0x3c, SDA, SCL); //0x3c for 128*32 OLED

const uint8_t resetOledPin      = 16;
const uint8_t powerOledPin      = 17;

const uint16_t OledDelayOff     = 5000;

long oledMillis                 = 0;

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
  initOLED();

  initWifi();
  initOTA();
  
  initMQTT();
  
  initPostal();

  lastMillis = millis();
  
  local_delay(50);

  //Initialize data
  newStart = true;
  oledMillis = millis() + OledDelayOff;
  
  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();

  runEverySecond();

  newStart = false;                                  //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
  runOTA();
  
  mqttActive = runMQTT();
  networkActive = checkNetwork();

  //mqttKeepRunning();                              //Not on this project

  mqttPublish();                                    //Run to check nothing has been received and needs to republish
  
  local_delay(10);
}

void runEverySecond()
{
  if (millis()-lastMillis < 1000 && !loraRead())
    return;

  unsigned long loopTime = millis();

  updatePublish = true;
  mqttPublish();                                     //Publish MQTT data

  oledUpdate();

  lastMillis = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}
