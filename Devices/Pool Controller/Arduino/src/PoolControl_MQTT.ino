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
  Name:     Pool Control (MQTT)
  Created:  2020/04/19
  Modified: 2020/05/21
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Pool%20Controller
*/

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
IPAddress gateway(GW1, GW2, GW3, GW4);            //Put your gateway IP address here
IPAddress subnet(SN1, SN2, SN3, SN4);             //Put your subnet mask here
IPAddress dnsIp(IP1, IP2, IP3, IP4);              //Put your DNS Server here

#define ENC28J60_SS_pin             53

bool networkActive                = false;        //WiFi connectivity status

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

#ifndef MEGA2560
  #define MEGA2560
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <avr/wdt.h>

const bool ENABLE_WDT             = true;

const uint8_t selfReset_pin       = 48;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
const uint8_t boardLedPin         = LED_BUILTIN;  //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert      = false;        //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   -99           //Initialization value to insure values updates

bool newStart                     = false;        //New start detection

long ledFlashDelay                = 0;            //Led flashing delay
long lastSecond                   = 0;
long lastMinute                   = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <PubSubClient.h>

EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
long lastMsg                      = 0;
char msg[50];

bool updatePublish                = false;
bool mqttActive                   = false;

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

int address_Drn                   = 0;
int address_Sld                   = 1;
int address_Htp                   = 2;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------

// --------------------------------------- I2C SENSORS DEFINES ----------------------------------------
const uint8_t sda                 = 20;                 //I2C Data pin
const uint8_t scl                 = 21;                 //I2C Clock pin


// ------------------------------------------ RELAYS DEFINES ------------------------------------------
const uint8_t relayQty            = 16;
const uint8_t relayPin[relayQty]  = {27, 29, 31, 33, 35, 37, 39, 41, 26, 28, 30, 32, 34, 36, 38, 40};
bool relayCmd[relayQty]           = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

const uint8_t ValveControl_ptr    = 0;  //12Vdc   - Pin 1
//const uint8_t Unknown_ptr       = 1;  //12Vdc   - Pin 2
//const uint8_t Unknown_ptr       = 2;  //12Vdc   - Pin 3
//const uint8_t Unknown_ptr       = 3;  //12Vdc   - Pin 4
//const uint8_t Unknown_ptr       = 4;  //12Vdc   - Pin 5
//const uint8_t Unknown_ptr       = 5;  //12Vdc   - Pin 6
//const uint8_t Unknown_ptr       = 6;  //12Vdc   - Pin 7
//const uint8_t Unknown_ptr       = 7;  //12Vdc   - Pin 8
const uint8_t PoolFiller_ptr      = 8;  //12Vdc   - Pin 1
const uint8_t GardenWater_ptr     = 9;  //12Vdc   - Pin 2
const uint8_t DoorLock_ptr        = 10; //12Vdc   - Pin 3
const uint8_t DoorUnlock_ptr      = 11; //12Vdc   - Pin 4
//const uint8_t Unknown_ptr       = 12; //120Vac  - Pin 5
const uint8_t PondPump_ptr        = 13; //120Vac  - Pin 6
const uint8_t PoolHeat_ptr        = 14; //120Vac  - Pin 7
const uint8_t PoolPump_ptr        = 15; //120Vac  - Pin 8

// -------------------------------------- ANALOG SENSORS DEFINES --------------------------------------
/*
 * Boards       Max Volts     Pins          Max Resolution
 * Mega,        5 Volts       A0 to A14     10 bits
 * Mega2560,
 * MegaADK
 */

const uint8_t poolPH_pin          = A0;
const uint8_t poolPress_pin       = A1;
const uint8_t poolTempIn_pin      = A2;
const uint8_t poolTempOut_pin     = A3;
const uint8_t poolPumpTemp_pin    = A4;

uint16_t poolPH                   = 0;
uint16_t poolTempIn               = 0;
uint16_t poolTempOut              = 0;
uint16_t poolPress                = 0;
uint16_t poolPumpTemp             = 0;

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
const uint8_t poolLvlHi_pin       = 12;
const uint8_t poolLvlLo_pin       = 11;
const uint8_t poolLvlSpr_pin      = 10;

bool poolLvlHi                    = false;
bool poolLvlLo                    = false;

//Door/Lock
const uint8_t poolCabinDoor_pin   = 8;
const uint8_t poolCabinLocked_pin = 9;

bool poolCabinDoor                = false;
bool poolCabinLocked              = false;

unsigned long lockUnlockTmr       = 0;
const uint16_t lockUnlockHold     = 2000;

bool lockRequest                  = false;        //Lock request received from MQTT
bool unlockRequest                = false;        //Unlock request received from MQTT

bool Motor1Output                 = false;        //Motor output bit
bool Motor2Output                 = false;        //Motor output bit

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
#include <Wire.h>
#include "DHT.h"

//#define DHTTYPE                   DHT11         // DHT 11
#define DHTTYPE                     DHT22         // DHT 22 (AM2302), AM2321
//#define DHTTYPE                   DHT21         // DHT 21 (AM2301)

const uint8_t DHT_pin             = 19;           //DHT Temp/Hum sensor (1-wire)

float DHTTempOut                  = initValue;    //DHT temperature output to MQTT
float DHTHumOut                   = initValue;    //DHT humidity output to MQTT

DHT dht(DHT_pin, DHTTYPE);                         //Create DHT object

// ------------------------------------ DHT INTERNAL SENSOR DEFINES -----------------------------------
const uint8_t DHTInternal_pin     = 49;           //DHT Temp/Hum sensor (1-wire)

float DHTTempIn                   = initValue;    //DHT temperature output to MQTT
float DHTHumIn                    = initValue;    //DHT humidity output to MQTT

DHT dhtInternal(DHTInternal_pin, DHTTYPE);        //Create DHT object

//-------------------------------------- SERVOS CONTROLS DEFINES --------------------------------------
#include <Servo.h>

const uint8_t flowSkimDrain_pin   = 7;            //Water flow control to heat pump
const uint8_t flowHeater_pin      = 6;            //Water flow control to slide
const uint8_t flowSlide_pin       = 5;            //Water flow control skimmer/drain

int flowSkimDrain                 = 0;
int flowHeater                    = 0;
int flowSlide                     = 0;

Servo flowSkimDrain_servo;                        // Create servo object to control a servo
Servo flowHeater_servo;                           // Create servo object to control a servo
Servo flowSlide_servo;                            // Create servo object to control a servo

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

  initEEPROM();
  initSensors();

  initEthernet();
  initMQTT();

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

  newStart = false;                                //First scan bit
}

void runEveryScan()
{
  wdt_reset();
  
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
