/*
  Garage Clock/Temperature - WiFi
  
  ESP8266-07 Modbus Slave for the TM1637 displays

  Program with following settings (status line / IDE):
    Board: Generic ESP8266 Module, 
    Crystal Frequency: 26MHz,
    Flash: 40MHz, 
    CPU: 80MHz, 
    Flash Mode: QIO, 
    Upload speed: 115200, 
    Flash size: 512k (no SPIFFS), 
    Reset method: ck, Disabled, none
    Erase Flash: All flash contents,
    Builtin LED: 2
*/

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- MODBUS MASTER DEFINES ---------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Ethernet.h>
#include <ESP8266WiFi.h>
#include <ModbusTCP.h>

#define ESP8266 1

//Modbus addresses
#define TIME_START            2000
#define TIME_LENGTH           6
#define TEMP_START            10
#define TEMP_LENGTH           8

uint16_t receivedData[10];                     //Data from Modbus Server

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"
IPAddress serverIP(SIP1, SIP2, SIP3, SIP4);     //Put IP address of PLC here
IPAddress ip(IP1, IP2, IP3, IP4);               //Put the current device IP address here
IPAddress gateway(GW1, GW2, GW3, GW4);          //Put your gateway IP address here
IPAddress subnet(SN1, SN2, SN3, SN4);           //Put your subnetmask here

const char ssid[]             = WIFI_SSID;      //Put your SSID here in between ""
const char password[]         = WIFI_PASSWD;    //Put your WiFi password here in between ""
bool wifiActive               = false;          //WiFi connectivity status
bool modbusActive             = false;          //Modbus connectivity status

//ModbusTCP Server object
ModbusTCP node(1);                              //Parameter is unit identifier

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
#define blueLedPin            LED_BUILTIN       //Pin 1 on ESP-01, pin 2 on ESP-12E

//Variables
#define initValue             255               //Initialization value to insure values updates

#define DEBUG                                   //Define to show serial prints

#ifndef ESP8266
  #define ESP8266
#endif
//ADC_MODE(ADC_VCC); //Read Vcc on ADC input

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- NEOPIXEL STRIP DEFINES --------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//Used Pins
#define neo_Pin               14
#define neo_Pixels            288

// Parameter 1 = Number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(neo_Pixels, neo_Pin, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

bool changeColor              = false;          //Change display color 

//Rainbow indexes and variables
unsigned long nextRainbow     = 0;
unsigned long nextSwitch      = 0;
uint16_t rainbowIndex         = 0;
uint8_t displayFeature        = 96; //Initialize displays
uint8_t loopIndex             = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- TM1637 - TIME DISPLAY ----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>            // http://playground.arduino.cc/code/time (Margolis 1.5.0)
#include "TM1637_JG.h"

//Used Pins
#define clockCLK_Pin          15
#define clockDIO_Pin          5

#define clockDigits           6

//Variables
uint16_t Year                 = initValue;
uint8_t Month                 = initValue;
uint8_t Day                   = initValue;
uint8_t Hour                  = initValue;
uint8_t Minute                = initValue;
uint8_t Second                = initValue;
uint8_t DOW                   = initValue;

uint8_t prevHour              = initValue;
uint8_t prevMinute            = initValue;
uint8_t prevSecond            = initValue;

bool prevMinuteONS            = false;          //One shot for every minute update
bool localTimeValid           = false;          //Detect that local time is valid
bool plcTimeInvalid           = true;           //Detects that time has been collected successfuly
bool newStart                 = true;           //First scan bit

int8_t digitTime[clockDigits];                  //Array for displaying digits, the first number in the array will be displayed on the right
int8_t pointTime[clockDigits];                  //Array for displaying points, the first point in the array will be displayed on the right

TM1637_JG timeDisplay(clockCLK_Pin, clockDIO_Pin, clockDigits);

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- TM1637 - TEMP DISPLAY ----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include "TM1637_JG.h"

//Used Pins
#define tempCLK_Pin           13
#define tempDIO_Pin           12

#define tempDigits            4

//Variables
float garageTempC             = initValue;
float garageTempF             = initValue;
float garageHumidity          = initValue;

bool prevTempONS              = false;          //One shot for text changing sequence
int8_t digitTemp[tempDigits];                   //Array for displaying digits, the first number in the array will be displayed on the right
int8_t pointTemp[tempDigits];                   //Array for displaying points, the first point in the array will be displayed on the right

TM1637_JG tempDisplay(tempCLK_Pin, tempDIO_Pin, tempDigits);

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ LIGHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define LightSensor           A0

int lightLevel                = 0;

unsigned long lastLightUpdate = 0;
#define lightUpdate           60000
#define lightLevelOff         200

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- BUTTONS -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
unsigned long forceTimeON     = 0;              //Timestamp of force ON request event
bool forceON                  = false;          //Display is forced ON no matter the time of day
bool forceOFF                 = false;          //Display is forced OFF no matter the time of day

unsigned long lastButtonPress = 0;              //Timestamp of button pressed event

bool buttonActive             = false;          //Button is currently pressed (used to one shot timer)
bool shortPressActive         = false;          //Button has reached short press delay
bool medPressActive           = false;          //Button has reached medium press delay
bool longPressActive          = false;          //Button has reached long press delay

bool colorAutoSwitch          = false;          //Displays are in auto color switch mode (scrolls between modes)

#define shortPressTime        1000
#define medPressTime          2000
#define longPressTime         5000
#define extraTime             3600000

#define buttonPin             16

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- SETUP ------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(115200);
  #endif
  
  //Disable watchdog for bootup process
  ESP.wdtDisable();
  
  //Set Pin modes
  pinMode(blueLedPin, OUTPUT);

  //Start WiFi protocol
  WiFi.config(ip, gateway, subnet);
  delay(100);
  WiFi.begin(ssid, password);

  //Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED)
    flashBlueLed(100, 1);

  wifiActive = true;

  //Start master communication
  node.setServerIPAddress(serverIP);

  //Displays Setup
  timeDisplay.init();
  tempDisplay.init();

  delay(50);                //Wait for all data to be ready

  //NeoPixels Setup
  strip.begin();
  strip.show();             // Initialize all pixels to 'off'

  delay(50);                //Wait for all data to be ready

  //Enable watchdog
  ESP.wdtEnable(1000);      //Code should be able to run within 1 second

  bootDisplays();

  //Initialize PLC data
  plcTimeInvalid = true;
  newStart = true;
  modbusActive = false;
  localTimeValid = false;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  ESP.wdtFeed();
  
  if (WiFi.status() != WL_CONNECTED)
  {
    flashBlueLed(100, 1);
    wifiActive = false;
  }
  else
    wifiActive = true;
    
  readSensors();            //Read sensors (buttons, etc.)
  setDisplayBrigtness(lightLevel);

  autoColorChanging();

  readModbusAll();          //Read all modbus data

  updateTime();             //Display current time
  updateTemp();             //Display garage temperature/humidity

  //Short flash every 5 seconds when everything is ok
  if (prevSecond != Second && (Second%5) == 0)
    flashBlueLed(10, 1);

  prevSecond = Second;
  prevMinute = Minute;
  //prevHour = Hour;        //Not needed
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- INIT FUNCTIONS --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void bootDisplays()
{
  int maxDigits = max(clockDigits, tempDigits); //Find the largest display

  //Turn all digit segments ON and turn all dots on
  for(int i=0; i < maxDigits; i++)
  {
    if(i < clockDigits)
    {
      digitTime[i] = 8;
      pointTime[i] = POINT_ON;
    }

    if(i < tempDigits)
    {
      digitTemp[i] = 8;
      pointTemp[i] = POINT_ON;
    }
  }

  setDisplayBrigtness(BRIGHTEST);
  timeDisplay.display(digitTime, pointTime);
  tempDisplay.display(digitTemp, pointTemp);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- TIME DISPLAY FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
// Array for displaying digits, the first number (myArray[0]) will be displayed on the right
void updateTime()
{  
  bool newTimeData = false;

  //Update local time once per second from internal RTC
  if (prevSecond != second())
  {
    Year = year();
    Month = month();
    Day = day();
    Hour = hour();
    Minute = minute();
    Second = second();
    DOW = weekday();         //Day of the week (1-7), Sunday is day 1
    newTimeData = true;
  }

  //If no new time available, return
  if (!newTimeData)
    return;

  //If minute hasn't changed, unless a new boot is detected, return
  //if (Minute == prevMinute && !plcTimeInvalid)
  //  return;

  // Send the time to display, but only if there's no error in values
  if (Hour < 0 || Hour > 23 || Minute < 0 || Minute > 59 || Second < 0 || Second > 59)
    return;

  //Send the time to display
  sendTimeToDisplay();

  newTimeData = false;        //Clear new time from PLC flag
}

//Read time data from ModbusTCP Server
boolean getTime()
{
  //Read time data from PLC
  if(!readModbusServerHreg(TIME_START, TIME_LENGTH))
    return false;
    
  int extraSeconds = 0;
    
  int YearMB    = constrain(receivedData[0], 1970, 2500);
  int MonthMB   = constrain(receivedData[1], 1, 12);
  int DayMB     = constrain(receivedData[2], 1, 31);
  int HourMB    = constrain(receivedData[3], 0, 23);
  int MinuteMB  = constrain(receivedData[4], 0, 59);
  int SecondMB  = constrain(receivedData[5]+extraSeconds, 0, 59); //Add extra seconds to compensate for communication delay

  setTime(HourMB, MinuteMB, SecondMB, DayMB, MonthMB, YearMB);

  //Year data not updated
  if (year() <= 1970)
    return false;

  return true;
}

//Send the time to the display
void sendTimeToDisplay()
{
  //Set displays data if time is correct
  digitTime[5] = Hour / 10;
  digitTime[4] = Hour % 10;
  digitTime[3] = Minute / 10;
  digitTime[2] = Minute % 10;
  digitTime[1] = Second / 10;
  digitTime[0] = Second % 10;

  //Set decimal points every even seconds
  int moduloSecs = ((Second % 2)==0) ? POINT_ON : POINT_OFF;
  for (int i = 0; i < clockDigits-2; i++)
    pointTime[i] = moduloSecs;

  //Clear extra decimal points
  for (int i = clockDigits-2; i < clockDigits; i++)
    pointTime[i] = POINT_OFF;

  //Send data to display
  timeDisplay.display(digitTime, pointTime);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- TEMP DISPLAY FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
// Array for displaying digits, the first number (myArray[0]) will be displayed on the right
void updateTemp()
{
  //update once per second
  if (Second == prevSecond && !newStart)
    return;
    
  int moduloSecs = newStart ? 0 : Second % 10;

  if (moduloSecs != 0 && moduloSecs != 4 && moduloSecs != 7)
    prevTempONS = false;
  
  if (prevTempONS)
    return;

  //Display temperature in °C
  if (moduloSecs == 0)
  {
    tempDisplay.ForceUppercase(true);
    setDataForDisplay(garageTempC, "C");
    prevTempONS = true;
  }
  //Display temperature in °F
  else if (moduloSecs == 4)
  {
    tempDisplay.ForceUppercase(true);
    setDataForDisplay(garageTempF, "F");
    prevTempONS = true;
  }
  //Display humidity level in %
  else if (moduloSecs == 7)
  {
    tempDisplay.ForceUppercase(false);
    setDataForDisplay(garageHumidity, "h");
    prevTempONS = true;
  }

  //Send data to display
  tempDisplay.display(digitTemp, pointTemp);
}

//Read garage temperature data from ModbusTCP Server
boolean getTemp()
{
  if(!readModbusServerHreg(TEMP_START, TEMP_LENGTH))
    return false;

  receivedData[0] = -258;
  receivedData[1] = 536;
    
  garageTempC = float(receivedData[0])/10.0;
  garageTempF = celciusToFahrenheit(garageTempC);
  garageHumidity = float(receivedData[1])/10.0;

  return true;
}

// Array for displaying digits, the first number (myArray[0]) will be displayed on the right
void setDataForDisplay(float value, String units)
{
  int newValue;
  if (value > 99.9 || value < -9.9)
    newValue = abs(value);
  else
    newValue = abs(value*10);

  //Clear decimal points
  for (int i = 0; i < tempDigits; i++)
    pointTemp[i] = POINT_OFF;
    
  if (value < 10)
  {
    if (value < 0)
      digitTemp[3] = tempDisplay.FindIndexOfChar("-");
    else
      digitTemp[3] = tempDisplay.FindIndexOfChar(" ");
    digitTemp[2] = newValue/10;
  }
  else
  {
    digitTemp[3] = newValue/100;
    digitTemp[2] = (newValue - (digitTemp[0]*100)) / 10;
  }
  
  digitTemp[1] = newValue%10;
  digitTemp[0] = tempDisplay.FindIndexOfChar(units);

  //Set decimal point if necessary
  if (value < 100 && value > -10)
    pointTemp[2] = POINT_ON;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- PLC FUNCTION -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Try to read the time, if it works, read all other registers right away
void readModbusAll()
{
  int secondReadTrigger = 51; //Any seconds value between 10 and 59, preferably not a value divisible by 10
  secondReadTrigger = constrain(secondReadTrigger, 10, 59);
  
  if (((second() == secondReadTrigger) && !prevMinuteONS && wifiActive) || newStart)
  {
    ESP.wdtFeed();                //Clear watchdog
    plcTimeInvalid = !getTime();  //Get current time from PLC
    ESP.wdtFeed();                //Clear watchdog

    modbusActive = checkModbusConnection();

    if (modbusActive)
    {
      //Other registers to read goes here (such as getTemp())
      getTemp();
    }
    else
      flashBlueLed(50, 2);

    prevMinuteONS = true;
  }

  localTimeValid = (year() != 1970);  //Once time is updated once, consider time as valid forever

  if (Second < 10)
    prevMinuteONS = false;   //Reset one shot after few seconds
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readSensors()
{
  if (lastLightUpdate > millis()) // Rollover
    lastLightUpdate = 0;
    
  if (((millis() - lastLightUpdate) > lightUpdate) || (lastLightUpdate == 0) || (lastLightUpdate == initValue))
  {
    lightLevel = analogRead(LightSensor);
    yield();
    lastLightUpdate = millis();
  }
  
  if (lightLevel < lightLevelOff)
    lightLevel = 0;
  else
    lightLevel /= 128;
}

void setDisplayBrigtness(int value)
{
  value = constrain(value, 0, 7);
  
  tempDisplay.setBrightness(value);
  timeDisplay.setBrightness(value);
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- Utility functions ----------------------------------------
// ----------------------------------------------------------------------------------------------------

void setBlueLED(boolean newState)
{
  digitalWrite(blueLedPin, newState);
}

void flashBlueLed(int delayFlash, int qtyFlash)
{
  for (int i = 0; i < qtyFlash; i++)
  {
    setBlueLED(false);
    delay(delayFlash);
    setBlueLED(true);
    delay(delayFlash);
  }
}

float celciusToFahrenheit(float celcius)
{
  return ((celcius * (9/5)) + 32);
}

float fahrenheitToCelcius(float fahrenheit)
{
  return ((fahrenheit - 32) * (5/9));
}
