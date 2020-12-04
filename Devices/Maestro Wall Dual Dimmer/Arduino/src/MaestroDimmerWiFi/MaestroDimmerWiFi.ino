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
 Name:    Maestro Dimmer Kit - WiFi Canopy
 Created: 2020/11/20
 Created: 2020/12/01
 Author:  gauthier_j100@hotmail.com / SupremeSports
 GitHub:  https://github.com/SupremeSports/
*/

const char* version               = "v:3.0.1";
const char* date                  = "2020/12/01";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <WiFi.h>

#define ESP32                       32

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnetmask here

bool networkActive                = false;        //WiFi connectivity status
int rssiPercent                   = 0;            //WiFi signal strength in percent
int rssi                          = 0;            //WiFi signal strength in dBm

bool testStandalone               = true;
uint8_t pinStandalone             = 36;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
hw_timer_t *watchdogTimer         = NULL;         //Watchdog implementation
#define LWD_TIMEOUT                 5000          //WDT Value (ms)

#include <rom/rtc.h>

const uint8_t selfResetPin        = 25;           //Set to output ONLY when ready to reset

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

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

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin         = 4;            //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
bool enableBoardLED               = false;        //If true, LED will flash to indicate status

//Variables
#define initValue                 -1              //Initialization value to insure values updates

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
// ------------------------------------------ NRF24 DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
//RH_NRF24 nrf24(12, 5);
RH_NRF24 nrf24(16, 17); //(CE, CSN)

uint8_t nrfChannel                = 2;
uint16_t switchID                 = 0x0003;
uint16_t switchRcvdID             = 0;

long lastDataNRF                  = 0;                            //Last communication time
bool sendDataNRF                  = false;

uint8_t PWM_OFFSET                = 0;                            //% Light level percentage

bool permissionSendNRF            = false;                        //Prevent operation before reading EEPROM from switch

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- I2C DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include <Wire.h>
//#include <WireSlave.h>

#define SDA_PIN                     21
#define SCL_PIN                     22
#define I2C_SLAVE_ADDR              0x04

// set the max number of bytes the slave will send.
// if the slave send more bytes, they will still be read
// but the WireSlaveRequest will perform more reads
// until a whole packet is read
#define MAX_SLAVE_RESPONSE_LENGTH   32

#define I2C_IRQ                     5

uint8_t heartbeatIn               = 0;
uint8_t heartbeatOut              = 0;

long lastDataI2C                  = 0;                            //Last communication time
bool sendDataI2C                  = false;

uint8_t optionData                = 0;
bool enableRemoteLED              = false;
#define LED_BIT                     0                             //Enable/Disable onboard LED 1=ON

bool permissionSendI2C            = false;                        //Prevent operation before reading EEPROM from switch

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- LIGHT & FAN DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------
typedef struct
{
  uint8_t level;
  uint8_t out;
  bool state;
  bool full;
  bool fade;
} dimmer;

#define STATE_BIT                   7     //Output ON or OFF regardless of dim level
#define FADE_BIT                    6     //Enable fading on this output
#define FULL_BIT                    5     //Force to be at full while remembering the dim level

// ---------------------------------------- LAMP DIMMER DEFINES ---------------------------------------
dimmer lamp;
dimmer lampMQTT;
dimmer lampNRF;

#define lampMin                     1
#define lampMax                     7

// ---------------------------------------- FAN DIMMER DEFINES ----------------------------------------
dimmer fan;
dimmer fanMQTT;
dimmer fanNRF;

#define fanMin                      1
#define fanMax                      7

// ------------------------------------------ OTHERS DEFINES ------------------------------------------
bool newDataMQTT                  = false;
bool updateMQTT                   = false;

bool newDataNRF                   = false;
bool updateNRF                    = false;

bool standaloneMode               = false;

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t voltage5V_pin       = A5;

float voltage5VDimmer             = 0.0;
float voltage5VSwitch             = 0.0;

//R1  = 10k
//R2  = 15k
//Vr2 = 3.30V @ 5.50V
//Thorical ratio = 0.6
float voltage5VRatio              = 1.10000;      //Voltage divider ratio [0 => 5.5]Vdc

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

  initData();
  initNRF();
  initI2C();

  initWifi();
  initOTA();
  
  initMQTT();

  lastSecond = millis();

  local_delay(50);                                //Wait for all data to be ready
  
  //Initialize data
  newStart = true;
  permissionSendI2C = false;
  permissionSendNRF = false;

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
  runStandalone();

  networkActive = checkNetwork();
  mqttActive = runMQTT();
  //networkActive = checkNetwork();

  mqttPublish();                                  //Run to check nothing has been received and needs to republish

  local_delay(5);
}

void runEverySecond()
{
  if (millis()-lastSecond < 1000)
    return;

  #ifdef DEBUG
    unsigned long loopTime = millis();
    
    updatePublish = true;
    mqttPublish();                                  //Publish MQTT data
  
    lastSecond = millis();
  
    loopTime = millis() - loopTime;
    /*Sprint("Process time: ");
    Sprint(loopTime);
    Sprintln("ms");*/
  #else
    updatePublish = true;
    mqttPublish();                                  //Publish MQTT data
    
    lastSecond = millis();
  #endif
}

void runEveryMinute()
{
  if (millis()-lastMinute < 60000)
    return;

  #ifdef DEBUG
    unsigned long loopTime = millis();

    sendLightState();
    sendFanState();
  
    lastMinute = millis();
  
    loopTime = millis() - loopTime;
    Sprint("Process time (minute): ");
    Sprint(loopTime);
    Sprintln("ms");
  #else
    sendLightState();
    sendFanState();
    
    lastMinute = millis();
  #endif
}
