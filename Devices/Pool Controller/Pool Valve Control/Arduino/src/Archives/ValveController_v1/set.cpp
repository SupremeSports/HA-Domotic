#include "arduino.h"
#include <EEPROM.h>

#include "defines.h"
#include "variables.h"

#include "SET.h"

setclass::setclass(){}

//SERIAL PORT
//  This function starts the serial port if DEBUG_ENABLED is defined
void setclass::serial()
{
  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
    Serial.println("[START]");
  #endif
}

//SET PINS
//  This function sets the inputs and outputs
void setclass::setpins()
{
  pinMode(BUTTON_UP, INPUT_PULLUP);     //Digital
  pinMode(BUTTON_DN, INPUT_PULLUP);     //Digital
  pinMode(BUTTON_LK, INPUT_PULLUP);     //Digital
}

//EEPROM READ
//  This function reads all settings from EEPROM and adjust settings accordingly
//  If EEPROM_ENABLED is not defined, default values are used and will reset each time the unit is power cycled
void setclass::eeprom_read()
{
  #ifdef EEPROM_ENABLED
    //READ DATA FROM EEPROM
    
  #else
    //DEFAULT VALUES IF ETHERNET IS DISABLED

  #endif
}

//EEPROM WRITE
//  This function saves all settings to EEPROM whenever save_eeprom is requested (except when an alarm is present) 
//  Data is only updated, which means it's not written to EEPROM if its already the same value (saving EEPROM life)
void setclass::eeprom_write()
{
  #ifdef EEPROM_ENABLED
    //WRITE DATA TO EEPROM
    
  #endif
}

