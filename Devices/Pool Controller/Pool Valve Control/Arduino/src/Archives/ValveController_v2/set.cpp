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
    //Temperature data is lowered since it cannot accept values larger than 255
    //Temperature has to be multplied by 10 to accept decimal
    //Process reverse operation while saving data 
    temp_req = TEMP_MIN + ((float)EEPROM.read(EEPROM_TEMP)/10.0);
    Pot_Mtr[Mtr_1] = (int)EEPROM.read(EEPROM_POT1);
    Pot_Mtr[Mtr_2] = (int)EEPROM.read(EEPROM_POT2);
    Pot_Mtr[Mtr_3] = (int)EEPROM.read(EEPROM_POT3);

    int word_read = (int)EEPROM.read(EEPROM_BITS);
    disp_locked = bitRead(word_read, EEPROM_LOCK);
    Sw_Mtr[Mtr_1] = bitRead(word_read, EEPROM_SW1);
    Sw_Mtr[Mtr_2] = bitRead(word_read, EEPROM_SW2);
    Sw_Mtr[Mtr_3] = bitRead(word_read, EEPROM_SW3);
    heatpump_enable = bitRead(word_read, EEPROM_HEAT);
    solar_enable = bitRead(word_read, EEPROM_SOL);
    
    heat_enable = heatpump_enable || solar_enable ? true : false;
    
  #else
    //DEFAULT VALUES IF ETHERNET IS DISABLED
    temp_req = TEMP_DEF;

    Pot_Mtr[Mtr_1] = 0;
    Pot_Mtr[Mtr_2] = 0;
    Pot_Mtr[Mtr_3] = 50;

    disp_locked = false;
    Sw_Mtr[Mtr_1] = false;
    Sw_Mtr[Mtr_2] = false;
    Sw_Mtr[Mtr_3] = false;
    heatpump_enable = true;
    solar_enable = true;
    
    heat_enable = true;
  #endif
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
    bitWrite(word_write, EEPROM_SW1, Sw_Mtr[0]);
    bitWrite(word_write, EEPROM_SW2, Sw_Mtr[1]);
    bitWrite(word_write, EEPROM_SW3, Sw_Mtr[2]);
    bitWrite(word_write, EEPROM_HEAT, heatpump_enable);
    bitWrite(word_write, EEPROM_SOL, solar_enable);

    EEPROM.update(EEPROM_TEMP, byte(save_temp_req));
    EEPROM.update(EEPROM_POT1, byte(Pot_Mtr[0]));
    EEPROM.update(EEPROM_POT2, byte(Pot_Mtr[1]));
    EEPROM.update(EEPROM_POT3, byte(Pot_Mtr[2]));
    EEPROM.update(EEPROM_BITS, byte(word_write));
  #endif
}

