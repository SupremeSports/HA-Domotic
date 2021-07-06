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
  Name:     Shed Main
  Created:  2021/06/12
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Shed%20Main
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

const char* version               = "v:1.4.3";
const char* date                  = "2021/06/16";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- Ethernet DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <UIPEthernet.h>

#define MEGA2560                    1
#define ENC28J60                    1

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

byte mac[]                        = {MC0,MC1,MC2,MC3,MC4,MC5};
IPAddress ip(IP1, IP2, IP3, IP4);                 //Put the current device IP address here
IPAddress gateway(IP1, IP2, IP3, IP4);            //Put your gateway IP address here
IPAddress subnet(SN1, SN2, SN3, SN4);             //Put your subnet mask here
IPAddress dnsIp(IP1, IP2, IP3, IP4);              //Put your DNS Server here

#define ENC28J60_SS_pin             53

bool networkActive                = false;        //WiFi connectivity status

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <avr/wdt.h>

const bool ENABLE_WDT             = true;

const uint8_t selfReset_pin       = 7;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define ENABLE_OTA

#ifdef ENABLE_OTA
  #define NO_OTA_PORT
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
const uint8_t boardLedPin         = LED_BUILTIN;  //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   -99           //Initialization value to insure values updates

long lastSecond                   = 0;

bool newStart                     = false;        //New start detection
bool ONS_1s                       = false;
bool ONS_5s                       = false;
bool ONS_10s                      = false;
bool ONS_1m                       = false;

bool PRE_5s                       = false;        //Comes on for the second before ONS_5s comes on

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

int address_LGT                   = 0;  //Light prev status

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

const uint16_t keepAlive          = 45;
const uint16_t packetSize         = 1500;
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

const int BUFFER_SIZE             = JSON_OBJECT_SIZE(200);
const int BUFFER_ARRAY_SIZE       = 512;

char message[BUFFER_ARRAY_SIZE];

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t sda                 = 20;           //I2C Data pin
const uint8_t scl                 = 21;           //I2C Clock pin

// ------------------------------------------ RELAYS DEFINES ------------------------------------------
const uint8_t relayQty            = 8;
const uint8_t relayPin[relayQty]  = {42, 43, 44, 45, 46, 47, 48, 49};
bool relayCmd[relayQty]           = {false, false, false, false, false, false, false, false};
bool relayOns[relayQty]           = {false, false, false, false, false, false, false, false};
float relayHld[relayQty]          = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
long relayTmr[relayQty]           = {0, 0, 0, 0, 0, 0, 0, 0};

//const uint8_t relay1_ptr        = 0;  //R1
//const uint8_t relay2_ptr        = 1;  //R2
//const uint8_t relay3_ptr        = 2;  //R3
//const uint8_t relay4_ptr        = 3;  //R4
//const uint8_t relay5_ptr        = 4;  //R5
const uint8_t lightPool_ptr       = 5;  //R6
const uint8_t lightOut_ptr        = 6;  //R7
const uint8_t lightInt_ptr        = 7;  //R8

// -------------------------------------- ANALOG SENSORS DEFINES --------------------------------------
//TODO

// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
const uint8_t voltage5V_pin       = A14;
const uint8_t voltage12V_pin      = A15;

float voltage5V                   = 0.0;
float voltage12V                  = 0.0;

//R1  = 1k
//R2  = 10k
//Vr2 = 5.00V @ 5.50V
//Thorical ratio = 0.90909
float voltage5VRatio              = 1.13087;      //Voltage divider ratio [0 => 5.5]Vdc

//R1  = 10k
//R2  = 5k6
//Vr2 = 5.00V @ 13.93V
//Thorical ratio = 2.78571
float voltage12VRatio             = 2.88890;      //Voltage divider ratio [0 => 13.93]Vdc

// -------------------------------------- DIGITAL SENSORS DEFINES -------------------------------------
const uint8_t mainDoorClosedPin   = 22;           //Main door closed
const uint8_t leftDoorsClosedPin  = 23;           //Left doors closed (dual sensor in series)
const uint8_t rightDoorsClosedPin = 24;           //Right doors closed (dual sensor in series)

bool mainDoorStatus               = false;        //Door status to MQTT (true=closed)
bool leftDoorsStatus              = false;        //Door status to MQTT (true=closed)
bool rightDoorsStatus             = false;        //Door status to MQTT (true=closed)

const uint8_t mainDoorLockPin     = 25;            //Main door locked

bool mainDoorLockStatus           = false;        //Lock status to MQTT (true=locked)

#define lightIntSwPin               19            //Physical switch input
#define lightIntSwPtr               0             //Pointer in struct toggle toggleSw[3]

typedef struct
{
  uint8_t swPin;
  uint8_t outPtr;
  bool state;
  bool swStts;
  bool swPrev;
} toggle;

toggle toggleSw[3];
  
// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
#include <Wire.h>
#include "DHT.h"

//#define DHTTYPE                   DHT11         // DHT 11
#define DHTTYPE                     DHT22         // DHT 22 (AM2302), AM2321
//#define DHTTYPE                   DHT21         // DHT 21 (AM2301)

//#define EXTERNAL_EN

#ifdef EXTERNAL_EN
  const uint8_t DHTPin              = ??;           //DHT Temp/Hum sensor (1-wire)
  
  float DHTTempOut                  = initValue;    //DHT temperature output to MQTT
  float DHTHumOut                   = initValue;    //DHT humidity output to MQTT
  
  DHT dht(DHTPin, DHTTYPE);                         //Create DHT object
#endif

// ------------------------------------ DHT INTERNAL SENSOR DEFINES -----------------------------------
#define INTERNAL_EN

#ifdef INTERNAL_EN
  const uint8_t DHTInternalPin      = 12;           //DHT Temp/Hum sensor (1-wire)
  
  float DHTTempIn                   = initValue;    //DHT temperature output to MQTT
  float DHTHumIn                    = initValue;    //DHT humidity output to MQTT
  
  DHT dhtInternal(DHTInternalPin, DHTTYPE);         //Create DHT object
#endif

// ---------------------------------------- SELF-RESET DEFINES ----------------------------------------
unsigned long resetDelay          = 0;
bool resetONS                     = false;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>              // TimeLib library is needed https://github.com/PaulStoffregen/Time
                                  // http://playground.arduino.cc/code/time (Margolis 1.5.0)

//Variables
uint8_t Second                    = initValue;
uint8_t Minute                    = initValue;

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

  initEEPROM(); //Init EEPROM after dimmer show to prevent destroying read data
  initSwitches();

  initSensors();
  
  initEthernet();
  initOTA();
  
  initMQTT();

  lastSecond = millis();

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

  newStart = false;                                //First scan bit
}

void runEveryScan()
{
  wdt_reset();

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

  //readSensors(false);                             //Read sensors (buttons, etc.)

  updatePublish = true;
  mqttPublish();                                  //Publish MQTT data

  lastSecond = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
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
  //sendRelayStates();
  forceUpdateMQTT(true);
  sendSensors();
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;

  ONS_10s = false;
    
  Sprintln("10 seconds");

  writeEEPROM();
}

void runEveryMinute()
{
  if (!ONS_1m)
    return;

  ONS_1m = false;

  unsigned long loopTime = millis();

  //TODO

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

  sendRelayStates();

  //sendDigAnStates();
  sendLockStts();
}
