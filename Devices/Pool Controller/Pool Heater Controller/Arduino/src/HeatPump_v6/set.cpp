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
  //Inputs
  pinMode(BUTTONS, INPUT);              //Analog
  pinMode(TEMP_LP, INPUT);              //Analog
  pinMode(TEMP_IN_WATER, INPUT);        //Analog
  pinMode(TEMP_OUT_WATER, INPUT);       //Analog
  pinMode(SWITCH_FLOW, INPUT_PULLUP);   //Digital
  pinMode(SWITCH_HP, INPUT_PULLUP);     //Digital
  pinMode(SWITCH_LP, INPUT_PULLUP);     //Digital

  //Outputs
  pinMode(RELAY_FAN_ON, OUTPUT);        //Digital
  pinMode(RELAY_FAN_HIGH, OUTPUT);      //Digital
  pinMode(RELAY_HEAT, OUTPUT);          //Digital

  prevMillis_heat = millis();           //Set heat pump startup delay
  seq_count = DISP_BASE_NOW;            //Jump to normal running display
}

//EEPROM READ
//  This function reads all settings from EEPROM and adjust settings accordingly
//  If EEPROM_ENABLED is not defined, default values are used and will reset each time the unit is power cycled
void setclass::eeprom_read()
{
  #ifdef EEPROM_ENABLED
    //READ DATA FROM EEPROM
    //Temperature data is lowered since it cannot accept values larger than 255
    //Temperature has to be multplied by 10 to accept decimal
    //Process reverse operation while saving data 
    temp_req = TEMP_MIN + ((float)EEPROM.read(EEPROM_TEMP)/10.0);
      
    int word_read = (int)EEPROM.read(EEPROM_BITS);
  
    disp_locked = bitRead(word_read, EEPROM_LOCK);
    spa_mode = bitRead(word_read, EEPROM_SPA);
    heat_enable = bitRead(word_read, EEPROM_HEAT);
  #else
    //DEFAULT VALUES IF ETHERNET IS DISABLED
    temp_req = TEMP_DEF;
    
    spa_mode = false;
    disp_locked = true;
    heat_enable = true;
  #endif

  if(temp_req < TEMP_MIN)
    temp_req = TEMP_DEF;
  
  temp_req = constrain(temp_req, TEMP_MIN+1, TEMP_MAX);                     //Make sure temperature is between range
}

//EEPROM WRITE
//  This function saves all settings to EEPROM whenever save_eeprom is requested (except when an alarm is present) 
//  Data is only updated, which means it's not written to EEPROM if its already the same value (saving EEPROM life)
void setclass::eeprom_write()
{
  #ifdef EEPROM_ENABLED
    //WRITE DATA TO EEPROM
    int save_temp_req = (temp_req*10)-(TEMP_MIN*10);

    int word_write = 0;
    bitWrite(word_write, EEPROM_LOCK, disp_locked);
    bitWrite(word_write, EEPROM_SPA, spa_mode);
    bitWrite(word_write, EEPROM_HEAT, heat_enable);

    EEPROM.update(EEPROM_TEMP, byte(save_temp_req));
    EEPROM.update(EEPROM_BITS, byte(word_write));
  #endif
}

setclass set = setclass();
