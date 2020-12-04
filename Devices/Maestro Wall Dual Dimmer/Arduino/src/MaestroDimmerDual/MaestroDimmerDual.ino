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
  Name:     Maestro Dimmer Kit - Dimmer Canopy
  Created:  2020/11/09
  Modified: 2020/11/30
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:   https://github.com/SupremeSports/
*/

const char* version               = "v:3.0.2";
const char* date                  = "2020/12/02";

#define NANO

bool newPowerON                   = false;

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

#define BAUDRATE                    115200

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin         = 3;

const bool boardLedPinRevert      = false;                        //If true, LED is on when output is low
bool enableBoardLED               = false;                        //If true, LED will flash to indicate status

//Variables
#define initValue                   -1                            //Initialization value to insure values updates

bool newStart                     = false;                        //New start detection

long lastSecond                   = 0;
long lastMinute                   = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- I2C DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Wire.h>

#define SDA_PIN A4
#define SCL_PIN A5
#define I2C_SLAVE_ADDR              0x04

// set the max number of bytes the slave will send.
// if the slave send more bytes, they will still be read
// but the WireSlaveRequest will perform more reads
// until a whole packet is read
#define MAX_I2C_LENGTH              32

char message[MAX_I2C_LENGTH];
uint8_t messageLength             = 0;

uint8_t heartbeatOut              = 0;
uint8_t heartbeatIn               = 0;

long lastDataI2C                  = 0;                            //Last communication time
const int maxDataI2CDelay         = 5000;

//Using pin #4 to detect an interrupt
//#define ENABLE_IRQ
#define I2C_IRQ                     4
bool irq_ons                      = false;

uint8_t optionData                = 0;
#define LED_BIT                     0                             //Enable/Disable onboard LED 1=ON

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
// --------------------------------------- LIGHT & FAN DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <dimmable_light.h> //Use AVR Boards 1.8.2 or lower

#define ZC_PIN                      2                             //Zerocross detection

typedef struct
{
  uint8_t level;
  uint8_t table[9];
  float act;
  float rate;
  bool state;
  bool full;
  bool fade;
} dimmer;

#define STATE_BIT                   7     //Output ON or OFF regardless of dim level
#define FADE_BIT                    6     //Enable fading on this output
#define FULL_BIT                    5     //Force to be at full while remembering the dim level

// ---------------------------------------- LAMP DIMMER DEFINES ---------------------------------------
dimmer lamp;
#define LAMP_PIN                    A2

//1-7 as per Maestro Switch levels
#define lampMin                     1
#define lampMax                     7

DimmableLight LAMP(LAMP_PIN);

// ---------------------------------------- FAN DIMMER DEFINES ----------------------------------------
dimmer fan;
#define FAN_PIN                     A3

//1-7 as per Maestro Switch levels
#define fanMin                      1
#define fanMax                      7

DimmableLight FAN(FAN_PIN);

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  initArduino();

  initWDT();

  initDimmer();

  initI2C();

  lastSecond = millis();
  
  //Initialize data
  newStart = true;

  Sprintln("Init completed...");
}

void loop()
{
  runEveryScan();
  //runEverySecond();
  //runEvery5seconds();
  //runEvery10seconds();
  //runEveryMinute();

  newStart = false;                                  //First scan bit
}

void runEveryScan()
{
  runDimmer();
  runI2C();

  #ifndef DEBUG
    //local_delay(100); //Compensate Sprint delays
  #endif
}
