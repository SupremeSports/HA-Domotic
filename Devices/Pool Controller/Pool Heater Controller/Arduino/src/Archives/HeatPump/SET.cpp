#include "arduino.h"
#include "defines.h"
#include "SET.h"
#include "variables.h"
#include <EEPROM.h>

setclass::setclass(){}

void setclass::SETPINS()
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
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_HEAT, OUTPUT);

  //READ DATA FROM EEPROM
  temp_req = (int)EEPROM.read(0);
  temp_req /= 10.0;
  temp_req += temp_min;
  temp_req = constrain(temp_req, temp_min, temp_max);                     //Make sure temperature is between range
  
  spalock_modes = (int)EEPROM.read(1);

  Serial.println(spalock_modes);

  if(spalock_modes >= 100)
  {
    heat_enable = true;
    spalock_modes -= 100;
  }
  if(spalock_modes >= 10)
  {
    disp_locked = true;
    spalock_modes -= 10;
  }

  if(spalock_modes >= 1)
  {
    spa_mode -= true;
    spalock_modes -= 1;
  }
  
  count = 85;
}

setclass SET = setclass();
