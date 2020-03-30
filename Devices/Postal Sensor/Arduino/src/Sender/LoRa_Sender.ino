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
  Name:     LoRa Sender - Postal
  Created:  2019/06/05
  Modified: 2019/11/16
  Author:   gauthier_j100@hotmail.com / SupremeSports
  GitHub:  
*/

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

#ifndef PROMINI
  #define PROMINI
#endif

#define continuousMode          false

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ SLEEP DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include "LowPower.h"

#define LDR                     true
const uint16_t newMailDelay   = 20000;

const int privateDoorPin      = 3;
const int mainDoorPin         = 4;

long lastMillis               = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

int address_ID                = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- LoRa DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <SPI.h>
#include <LoRa.h>

#define BAND                    915E6

#define DELIMITER_LORA          ":"
#define NEWMAIL                 "1"
#define NOMAIL                  "0"
#define MAILERROR               "ERROR"

int encryptKey                = 200;
int counter                   = 1;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------------- SETUP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(9600);
  #endif

  pinMode(privateDoorPin, INPUT_PULLUP);
  pinMode(mainDoorPin, INPUT_PULLUP);

  initLora();

  readEEPROM();
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- MAIN LOOP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  // Allow wake up pin to trigger interrupt on low.
  attachInterrupt(digitalPinToInterrupt(privateDoorPin), wakeUp, FALLING);

  if (!continuousMode)
  {
    Sprintln("Going back to sleep...");
    delay(100);
    
    // Enter power down state with ADC and BOD module disabled.
    // Wake up when wake up pin is low.
    
    LoRa.sleep();
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  }

  // Disable external pin interrupt on wake up pin.
  detachInterrupt(digitalPinToInterrupt(privateDoorPin));

  lastMillis = millis();                    //Reset timer for new mail

  delay(1000);                              //Wait to make sure it's not a signal glitch

  if (!digitalRead(privateDoorPin))
  {
    loraWrite(false);

    if (LDR)
      doorStatus();
  }
  
  if (continuousMode)
    delay(5000);
  else
    while(false);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- INIT FUNCTIONS --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  counter = EEPROM.read(address_ID);
  if (counter > 999)
    counter = 1;
}

void initLora()
{
  Sprintln("LoRa Sender");

  while (!LoRa.begin(BAND))
  {
    Sprintln("Starting LoRa failed!");
    delay(100);
  }

  LoRa.setSpreadingFactor(12);           // ranges from 6-12,default 7 see API docs
  Sprintln("LoRa init succeeded.");
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- LoRa Function --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loraWrite(bool mail)
{
  String dataOut = loraBuildString(mail);
  
  // send packet
  LoRa.beginPacket();
  LoRa.print(dataOut);
  LoRa.endPacket();

  counter++;
  if (counter > 999)
    counter = 1;

  EEPROM.update(address_ID, counter);
}

String loraBuildString(bool mail)
{
  Sprint("Sending packet: ");
  Sprintln(counter);

  String Vcc = String(readVcc()/1000.0);

  String dataOutput = String(counter);
  dataOutput += DELIMITER_LORA;

  bool fail = false;

  if (LDR)
    dataOutput += mail ? NEWMAIL : NOMAIL;
  else
  {
    if (!digitalRead(privateDoorPin))
      dataOutput += (!digitalRead(mainDoorPin)) ? NEWMAIL : NOMAIL;
    else
    {
      dataOutput = MAILERROR;
      fail = true;
    }
  }

  if (!fail)
  {
    dataOutput += DELIMITER_LORA;
    dataOutput += String(Vcc);
    dataOutput += DELIMITER_LORA; //always end with a delimiter
  
    Sprintln(dataOutput);
    dataOutput = EncryptDecrypt(dataOutput, encryptKey);
  }

  Sprintln(dataOutput);
  return dataOutput;
}

void doorStatus()
{
  while ((millis() - lastMillis) <= newMailDelay && !digitalRead(privateDoorPin))
    delay(1000);

  if (!digitalRead(privateDoorPin))
  {
    Sprintln("New Mail!");
    loraWrite(true);
  }
  else
    Sprintln("Mailbox Empty!");
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- READ VCC ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
long readVcc() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ENCRYPT/DECRYPT -----------------------------------------
// ----------------------------------------------------------------------------------------------------
String EncryptDecrypt(String szPlainText, int szEncryptionKey)  
{  
  String szInputStringBuild = szPlainText;
  String szOutStringBuild = "";
  char Textch;
  for (int iCount = 0; iCount < szPlainText.length(); iCount++)
  {  
    Textch = szInputStringBuild[iCount];
    Textch = (char)(Textch ^ szEncryptionKey);
    szOutStringBuild += Textch;
  }  
  
  return szOutStringBuild;
}

void wakeUp()
{
  // Just a handler for the pin interrupt.
}
