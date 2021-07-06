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
 Author:  gauthier_j100@hotmail.com / SupremeSports
 GitHub:  https://github.com/SupremeSports/HA-Domotic/blob/master/Devices/Maestro%20Wall%20Dual%20Dimmer
*/

/*
  ArduinoOTA            1.0.5
  UIPEthernet           2.0.9
  PubSubClient          2.8.0
  ArduinoJson           6.17.2
  Wire 
  DHT sensor library    1.4.1
  Board                 Arduino Mega 2560 (Optiboot)
*/

const char* version               = "v:2.3.4";
const char* date                  = "2021/07/01";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <WiFi.h>

#define ESP32                       32
//ADC_MODE(ADC_VCC);                              //Read Vcc on ADC input

#define Switch1     //SN: 00001
//#define Switch2     //SN: 00002
//#define Switch3     //SN: 00003

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gw(GW1, GW2, GW3, GW4);                 //Put your gateway IP address here
IPAddress sn(SN1, SN2, SN3, SN4);                 //Put your subnetmask here
IPAddress dns(GW1, GW2, GW3, GW4);                //Put your DNS here
IPAddress dns1 = (uint32_t)0x00000000;
IPAddress dns2 = (uint32_t)0x00000000;

bool networkActive                = false;        //WiFi connectivity status
int rssiPercent                   = 0;            //WiFi signal strength in percent
int rssi                          = 0;            //WiFi signal strength in dBm

bool standaloneMode               = false;
uint8_t pinStandalone             = 36;

bool testStandalone               = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
hw_timer_t *watchdogTimer         = NULL;         //Watchdog implementation
#define LWD_TIMEOUT                 5000          //WDT Value (ms)

//#include <rom/rtc.h>

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define ENABLE_OTA

#ifdef ENABLE_OTA
  #include <ESPmDNS.h>
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
const uint8_t boardLedPin         = 4;            //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
bool enableBoardLED               = false;        //If true, LED will flash to indicate status

//Variables
#define initValue                   -1            //Initialization value to insure values updates

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

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(100);
const int BUFFER_ARRAY_SIZE       = 255;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t sda                 = 21;          //I2C Data pin
const uint8_t scl                 = 22;          //I2C Clock pin

// ------------------------------------------- I2C DEFINES --------------------------------------------
#include <Arduino.h>
#include <Wire.h>

const uint8_t addressI2C          = 0x04;
const uint8_t bytesI2C            = 32;

const uint8_t hrtByte             = 0;
const uint8_t idByte              = 1;

const uint8_t irqI2C              = 5;

long lastDataI2C                  = 0;                            //Last communication time
bool sendDataI2C                  = false;

bool enableRemoteLED              = false;

bool permissionSendI2C            = false;                        //Prevent operation before reading EEPROM from switch

// ------------------------------------------ NRF24 DEFINES -------------------------------------------
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RH_NRF24_MAX_MESSAGE_LEN    28

RF24 radio(16, 17); // CE, CSN

#if defined(Switch1)
const byte pipes[][6] = {"00001","00002"};                        //Pipes should be unique and switchID should be equal to [0]
const uint16_t switchID           = 0x01;                         //Pipes should be unique and switchID should be equal to [0]
#elif defined(Switch2)
const byte pipes[][6] = {"00003","00004"};                        //Pipes should be unique and switchID should be equal to [0]
const uint16_t switchID           = 0x03;                         //Pipes should be unique and switchID should be equal to [0]
#elif defined(Switch3)
const byte pipes[][6] = {"00005","00006"};                        //Pipes should be unique and switchID should be equal to [0]
const uint16_t switchID           = 0x05;                         //Pipes should be unique and switchID should be equal to [0]
#else
#error You MUST define a SwitchID
#endif

const char* delim                 = ":";

long lastDataNRF                  = 0;
uint8_t heartbeatIn               = 0;
uint8_t heartbeatOut              = 0;

bool sendDataNRF                  = false;
bool permissionSendNRF            = false;                        //Prevent operation before reading EEPROM from switch

uint8_t optionData                = 0;
#define LED_BIT                     0                             //Enable/Disable onboard LED 1=ON

uint8_t PWM_OFFSET                = 0;                            //% Light level percentage

// --------------------------------------- LIGHT & FAN DEFINES ----------------------------------------
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

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
float DHTTempOut                  = initValue;    //DHT temperature output to MQTT
float DHTHumOut                   = initValue;    //DHT humidity output to MQTT

float DHTTempIn                   = initValue;    //DHT temperature output to MQTT
float DHTHumIn                    = initValue;    //DHT humidity output to MQTT

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t voltage5V_pin       = A5;

float voltage5VDimmer             = 0.0;
float voltage5VSwitch             = 0.0;

//R1  = 10k
//R2  = 15k
//Vr2 = 3.30V @ 5.50V
//Thorical ratio = 0.6
float voltage5VRatio              = 1.10000;      //Voltage divider ratio [0 => 5.5]Vdc

// ---------------------------------------- SELF-RESET DEFINES ----------------------------------------
const uint8_t selfResetPin        = 25;           //Set to output ONLY when ready to reset

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>              // TimeLib library is needed https://github.com/PaulStoffregen/Time
                                  // http://playground.arduino.cc/code/time (Margolis 1.5.0)

//Variables
uint8_t Second                    = initValue;
uint8_t Minute                    = initValue;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(BAUDRATE);
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
    Sprint("Process time: ");
    Sprint(loopTime);
    Sprintln("ms");

    //Sprintln();
    //Sprintln(heartbeatIn);
    //Sprintln(heartbeatOut);
    //Sprintln();
  #else
    updatePublish = true;
    mqttPublish();                                  //Publish MQTT data
    
    lastSecond = millis();
  #endif
}

void runEvery5seconds()
{
  if (!ONS_5s)
    return;
  
  ONS_5s = false;
  PRE_5s = false;

  #ifdef DEBUG
    Sprintln("5 seconds");
  #endif

  flashEvery5sec();

  readSensors(true);
  local_delay(10);
  sendSensors();
  local_delay(5);
  updatePublish = true;
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;
    
  ONS_10s = false;

  //After 10 seconds without communication with wall switch, force canopy to work without it through HA
  if (!permissionSendI2C)
  {
    permissionSendI2C = true;
    lampNRF.state = false;
    lampNRF.fade = true;

    fanNRF.state = false;
    fanNRF.fade = false;

    newDataNRF = true;
  }

  #ifdef DEBUG
    Sprintln("10 seconds");
  #endif
  //TODO
}

void runEveryMinute()
{
  if (!ONS_1m)
    return;

  ONS_1m = false;

  #ifdef DEBUG
    unsigned long loopTime = millis();

    sendLightState();
    sendFanState();
  
    loopTime = millis() - loopTime;
    Sprint("Process time (minute): ");
    Sprint(loopTime);
    Sprintln("ms");
  #else
    sendLightState();
    sendFanState();
  #endif
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
