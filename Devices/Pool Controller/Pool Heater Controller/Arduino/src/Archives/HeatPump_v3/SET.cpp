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
  pinMode(RELAY_HEAT_HIGH, OUTPUT);
  pinMode(RELAY_HEAT_ON, OUTPUT);
}

setclass SET = setclass();
