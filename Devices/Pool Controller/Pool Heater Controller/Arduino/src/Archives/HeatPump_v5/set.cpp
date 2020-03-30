#include "arduino.h"
#include <EEPROM.h>
#include <EtherCard.h>                                                                 // Ethernet controller library
#include <IPAddress.h>                                                                 // IPAddress controller library

#include "defines.h"
#include "variables.h"

#include "SET.h"
#include "show.h"

setclass::setclass(){}

void setclass::serial()
{
  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
    Serial.println("[START]");
  #endif
}

void setclass::setpins()
{
  //Inputs
  pinMode(BUTTONS, INPUT);
  pinMode(TEMP_LP, INPUT);
  pinMode(TEMP_IN_WATER, INPUT);
  pinMode(TEMP_OUT_WATER, INPUT);
  pinMode(SWITCH_FLOW, INPUT);
  pinMode(SWITCH_HP, INPUT);
  pinMode(SWITCH_LP, INPUT);

  //Outputs
  pinMode(RELAY_FAN_ON, OUTPUT);
  pinMode(RELAY_FAN_HIGH, OUTPUT);
  pinMode(RELAY_HEAT, OUTPUT);
}




void setclass::eeprom_read()
{
  #ifdef EEPROM_ENABLED
    //READ DATA FROM EEPROM
    //Temperature data is lowered since it cannot accept values larger than 255
    //Temperature has to be multplied by 10 to accept decimal
    //Process same modification while saving data 
    temp_req = TEMP_MIN + ((float)EEPROM.read(0)/10);
      
    int bit_read = (int)EEPROM.read(1);
  
    disp_locked = bitRead(bit_read, 0);
    spa_mode = bitRead(bit_read, 1);
    heat_enable = bitRead(bit_read, 2);
    //spare1 = bitRead(bit_read, 3);
    //spare2 = bitRead(bit_read, 4);
    //spare3 = bitRead(bit_read, 5);
    //spare4 = bitRead(bit_read, 6);
    //spare5 = bitRead(bit_read, 7);
  #else
    //DEFAULT VALUES
    temp_req = TEMP_MIN + 10;
    
    spa_mode = false;
    disp_locked = true;
    heat_enable = true;
    //spare1 = false;
    //spare2 = false;
    //spare3 = false;
    //spare4 = false;
    //spare5 = false;
  #endif
  
  temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);                     //Make sure temperature is between range
}

void setclass::eeprom_write()
{
  #ifdef EEPROM_ENABLED
    //WRITE DATA TO EEPROM
    int save_temp_req = (temp_req-TEMP_MIN)*10;

    Serial.println(temp_req);
    Serial.println(save_temp_req);

    int bit_write = 0;
    bitWrite(bit_write, 0, disp_locked);
    bitWrite(bit_write, 1, spa_mode);
    bitWrite(bit_write, 2, heat_enable);
    //bitWrite(bit_write, 3, spare1);
    //bitWrite(bit_write, 4, spare2);
    //bitWrite(bit_write, 5, spare3);
    //bitWrite(bit_write, 6, spare4);
    //bitWrite(bit_write, 7, spare5);

    EEPROM.update(0, byte(save_temp_req));
    EEPROM.update(1, byte(bit_write));
  #endif
}

setclass set = setclass();
