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
  Name:     Doorchime - Arduino Nano music player
  Created:  2020/03/23
  Modified: 2020/03/23
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/
*/

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

#ifndef NANO
  #define NANO
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin                   = LED_BUILTIN;    //Pin 1 on ESP-01, pin 2 on ESP-12E/ESP32
const bool boardLedPinRevert                = false;          //If true, LED is on when output is low
const bool enableBoardLED                   = false;          //If true, LED will flash to indicate status

//Variables
#define initValue                             -1              //Initialization value to insure values updates

bool newStart                               = false;          //New start detection

long ledFlashDelay                          = 0;              //Led flashing delay

long lastMillis                             = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- I2C DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Wire.h>

const uint8_t bytesI2C                      = 8;

const uint8_t addressI2C                    = 9;

const uint8_t hrtByte                       = 0;
const uint8_t idByte                        = 1;

int dataIn[bytesI2C];
int dataOut[bytesI2C];

bool newI2Crequest                          = false;

unsigned long lastRequest                   = 0;

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- AUDIO CONTROLLER -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include "SD.h"
#include "TMRpcm.h"
#include "SPI.h"

#define MAXSOUND                              255
#define MAXVOL                                7

const uint8_t SD_ChipSelectPin              = 10;
const uint8_t speakerOut                    = 9;
const uint8_t speakerEN                     = A0;

TMRpcm tmrpcm;

//Create wave files using 2 letters from aa.wav to ju.wav (1-255)
const char *alphabet[]                      = {"", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
const char *extension                       = ".wav";

int chrLngt                                 = 7;          // Buffer big enough for 2-character file name + ".wav/0"
char filename[7];

byte selectedSound                          = 3;          //DEFAULT = Auld Lang Syne
byte selectedVolume                         = 7;          //Volume [0..7]
byte startVolume                            = 0;
byte activeVolume                           = 0;

bool SD_CardOK                              = false;
bool ringRequested                          = false;

const uint8_t soundID                       = 0x01;

//Data IN from I2C
byte soundByte                              = 2;
byte volumeByte                             = 3;
byte ringByte                               = 4;

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- DEFAULT DOORBELL -----------------------------------------
// ----------------------------------------------------------------------------------------------------
// TONES  ==========================================
// Start by defining the relationship between
//       note, period, &  frequency.
#define  c4       3830    // 261 Hz
#define  cd4      3610    // 277 Hz
#define  d4       3400    // 294 Hz
#define  dd4      3215    // 311 Hz
#define  e4       3038    // 329 Hz
#define  ed4      2864    // 349 Hz
#define  f4       2864    // 349 Hz
#define  fd4      2703    // 370 Hz
#define  g4       2550    // 392 Hz
#define  gd4      2410    // 415 Hz
#define  a4       2272    // 440 Hz
#define  ad4      2146    // 466 Hz
#define  b4       2028    // 494 Hz
#define  bd4      1912    // 523 Hz
#define  c5       1912    // 523 Hz
// Define a special note, 'r', to represent a rest
#define  r           0

const char *dingdong_name                   = "dingdong";

const PROGMEM uint16_t dingdong[] = {  b4,  g4};
const PROGMEM uint16_t ddbeats[]  = {  64,  128};
uint16_t MAX_COUNT_DD                       = sizeof(dingdong) / 2; // Dingdong length, for looping.

const PROGMEM uint16_t melody[] = {  c4,  g4,  f4,  e4,  d4,  c5,   g4,  f4,  e4,  d4,  c5,   g4,  f4,  e4,  f4,  d4};
const PROGMEM uint16_t beats[]  = {  64,  64,  16,  16,  16,  64,   64,  16,  16,  16,  64,   64,  16,  16,  16, 128};
uint16_t MAX_COUNT                          = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo                                  = 10000;
// Set length of pause between notes
uint16_t pause                              = 1000;
// Loop variable to increase Rest length
uint16_t rest_count                         = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
uint16_t tone_                              = 0;
uint16_t beat                               = 0;
long duration                               = 0;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial);
  #endif

  initSensors();

  initI2C();

  initAudio();
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();
  
  runEverySecond();

  newStart = false;                            //First scan bit
}

void runEveryScan()
{
  readSensors();

  runI2C();
  playAudio();

  writeOutputs();

  delay(10);
}


void runEverySecond()
{
  if (millis()-lastMillis < 1000)
    return;

  unsigned long loopTime = millis();

  //playAudio();

  lastMillis = millis();

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}
