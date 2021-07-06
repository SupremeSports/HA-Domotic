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
  Name:     Maestro Dimmer Kit - Wall Switch
  Created:  2020/11/13
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/HA-Domotic/blob/master/Devices/Maestro%20Wall%20Dual%20Dimmer
*/

const char* version               = "v:2.3.0";
const char* date                  = "2021/06/27";

#define NANO

//SELECT ONLY ONE
#define Switch1     //SN: 00001
//#define Switch2     //SN: 00002
//#define Switch3     //SN: 00003

bool newPowerON                   = false;

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
const uint8_t boardLedPin         = A0;

const bool boardLedPinRevert      = false;                        //If true, LED is on when output is low
bool enableBoardLED               = false;                        //If true, LED will flash to indicate status

//Variables
#define initValue                   -1                            //Initialization value to insure values updates

bool newStart                     = false;                        //New start detection

long lastSecond                   = 0;
long lastMinute                   = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

int address_LMP                   = 0;  //Lamp Level (0-7) and embedded status bits
int address_FAN                   = 1;  //Fan Level (0-7) and embedded status bits
int address_PWM                   = 2;  //LEDs PWM level 20-100%

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ NRF24 DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RH_NRF24_MAX_MESSAGE_LEN    28

RF24 radio(9, 10); // CE, CSN

#if defined(Switch1)
const byte pipes[][6] = {"00001","00002"};                        //Pipes should be unique and switchID should be equal to [0]
uint16_t switchID                 = 0x01;                         //Pipes should be unique and switchID should be equal to [0]
#elif defined(Switch2)
const byte pipes[][6] = {"00003","00004"};                        //Pipes should be unique and switchID should be equal to [0]
uint16_t switchID                 = 0x03;                         //Pipes should be unique and switchID should be equal to [0]
#elif defined(Switch3)
const byte pipes[][6] = {"00005","00006"};                        //Pipes should be unique and switchID should be equal to [0]
uint16_t switchID                 = 0x05;                         //Pipes should be unique and switchID should be equal to [0]
#else
#error You MUST define a SwitchID
#endif

const char* delim                 = ":";

long lastDataNRF                  = 0;
uint8_t heartbeatOut              = 0;
uint8_t heartbeatIn               = 0;

bool sendDataNRF                  = false;
bool permissionSendNRF            = false;                        //Prevent operation before reading EEPROM from switch

uint8_t optionData                = 0;
#define LED_BIT                     0                             //Enable/Disable onboard LED 1=ON

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PCA9685 DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

const float PWM_ON                = 1.0;        //% When dimmer is ON at this level
const float PWM_DIM               = 0.15;       //% When dimmer is OFF at this level
const float PWM_OFF               = 0.008;      //% When dimmer in NOT at this level

const float PWM_MIN               = PWM_OFF;
const float PWM_MAX               = 1.0;
float PWM_OFFSET                  = PWM_MAX;    //% Light level percentage

Adafruit_PWMServoDriver pwm       = Adafruit_PWMServoDriver();

bool reversePWM                   = true;
uint8_t ledFadeDelay              = 250;
long lastFadeDelay                = 0;

uint8_t pin_lmpLeds[7]            = {7, 6, 5, 4, 2, 3, 1};
uint8_t act_lmpLeds[7];

uint8_t pin_fanLeds[7]            = {14, 13, 12, 11, 10, 9, 8};

int startupLevel                  = 0;
bool startupShow                  = false;
bool startupRev                   = false;

uint8_t pwmEnablePin              = A2;

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- WATCHDOG DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <avr/wdt.h>

#define ENABLE_WDT

#define WDT_DELAY                   WDTO_1S //WDTO_500MS is too short
const int wdtDelays[]             = {15, 30, 60, 120, 250, 500, 1, 2, 4, 8};

//Self-reset function
//#define USE_SELFRESET

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- SPECIFIC DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
typedef struct
{
  uint8_t level;
  uint8_t act;
  uint8_t in;
  uint8_t out;
  bool state;
  bool full;
  bool fade;
  bool dim;
} dimmer;

#define STATE_BIT                   7     //Output ON or OFF regardless of dim level
#define FADE_BIT                    6     //Enable fading on this output
#define FULL_BIT                    5     //Force to be at full while remembering the dim level

// ---------------------------------------- LAMP SWITCH DEFINES ---------------------------------------
dimmer lamp;

#define lampMin                     1
#define lampMax                     7

// ---------------------------------------- FAN SWITCH DEFINES ----------------------------------------
dimmer fan;

#define fanMin                      1
#define fanMax                      7

// --------------------------------------- BUTTONS/LEDS DEFINES ---------------------------------------
#include <PinButton.h>

int buttonPin[6]                  = {2, 3, 4, 5, 6, 7};

PinButton btnFanON(buttonPin[0]);
PinButton btnFanUP(buttonPin[1]);
PinButton btnFanDN(buttonPin[2]);
PinButton btnLmpUP(buttonPin[3]);
PinButton btnLmpON(buttonPin[4]);
PinButton btnLmpDN(buttonPin[5]);

bool buttonPressed                = false;

// ---------------------------------------- DHT SENSOR DEFINES ----------------------------------------
#include <Wire.h>
#include "DHT.h"

//#define DHTTYPE                   DHT11         // DHT 11
#define DHTTYPE                     DHT22         // DHT 22 (AM2302), AM2321
//#define DHTTYPE                   DHT21         // DHT 21 (AM2301)

#define EXTERNAL_EN

#ifdef EXTERNAL_EN
  const uint8_t DHTPin              = 8;            //DHT Temp/Hum sensor (1-wire)
  
  float DHTTempOut                  = 0;//initValue;    //DHT temperature output to MQTT
  float DHTHumOut                   = 0;//initValue;    //DHT humidity output to MQTT
  
  DHT dht(DHTPin, DHTTYPE);                         //Create DHT object
#endif

// ------------------------------------ DHT INTERNAL SENSOR DEFINES -----------------------------------
//#define INTERNAL_EN

#ifdef INTERNAL_EN
  const uint8_t DHTInternal_pin     = 49;           //DHT Temp/Hum sensor (1-wire)
  
  float DHTTempIn                   = 0;//initValue;    //DHT temperature output to MQTT
  float DHTHumIn                    = 0;//initValue;    //DHT humidity output to MQTT
  
  DHT dhtInternal(DHTInternal_pin, DHTTYPE);        //Create DHT object
#endif


// --------------------------------------- SELF VOLTAGE READING ---------------------------------------
#define ENABLE_VOLT

const uint8_t voltage5V_pin       = A1;

float voltage5V                   = 5.05;

//R1  = 1k
//R2  = 10k
//Vr2 = 5.00V @ 5.50V
//Theoretical ratio = 0.90909

float voltage5VRatio              = 0.90909;                      //Voltage divider ratio [0 => 5.5]Vdc

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup() 
{
  initArduino();
  
  initDimmer();
  
  initEEPROM(); //Init EEPROM after dimmer show to prevent destroying read data
  
  initSensors();

  initNRF();

  sendDataNRF = true;

  Sprintln("Init Completed");
}

void loop()
{
  runEveryScan();
  runEverySecond();
  runEveryMinute();
  
  newStart = false;
}

void runEveryScan()
{
  readSensors(false);
  
  checkNRF();
  sendNRF();

  runDimmer();
}

void runEverySecond()
{
  if (millis()-lastSecond < (voltage5V>0 ? 5000 : 1000))
    return;

  #ifdef DEBUG
    unsigned long loopTime = millis();
      
    readSensors(true);

    lastSecond = millis();
  
    loopTime = millis() - loopTime;
    Sprint("Process time (second): ");
    Sprint(loopTime);
    Sprintln("ms");
  #else
    readSensors(true);
    
    lastSecond = millis();
  #endif
}

void runEveryMinute()
{
  if (millis()-lastMinute < 10000) //30s is better
    return;

  #ifdef DEBUG
    unsigned long loopTime = millis();
  
    writeEEPROM();
  
    lastMinute = millis();
  
    loopTime = millis() - loopTime;
    Sprint("Process time (minute): ");
    Sprint(loopTime);
    Sprintln("ms");
  #else
    writeEEPROM();
  
    lastMinute = millis();
  #endif
}
