// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  readEEPROM();

  sendDataNRF = true;
  buttonPressed = true;
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_LMP = EEPROM.read(address_LMP);
  byte value_FAN = EEPROM.read(address_FAN);
  byte value_PWM = EEPROM.read(address_PWM);

  lamp.out = value_LMP;
  lamp.in = value_LMP;
  lamp.level = value_LMP;
  lamp.state = bitRead(lamp.level, STATE_BIT)==1;
  lamp.fade = bitRead(lamp.level, FADE_BIT)==1;
  lamp.full = bitRead(lamp.level, FULL_BIT)==1;
  
  fan.out = value_FAN;
  fan.in = value_FAN;
  fan.level = value_FAN;
  fan.state = bitRead(fan.level, STATE_BIT)==1;
  fan.fade = bitRead(fan.level, FADE_BIT)==1;
  fan.full = bitRead(fan.level, FULL_BIT)==1;
    
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
  {
    bitClear(lamp.level, i);
    bitClear(fan.level, i);
  }

  float value_PWM_F = float(value_PWM)/100.0F;

  if (value_PWM_F > PWM_MIN && value_PWM_F <= PWM_MAX)
    PWM_OFFSET = value_PWM_F;

  Sprintln("Read EEPROM: ");
  Sprintln(lamp.out);
  Sprintln(fan.out);
  Sprintln(PWM_OFFSET);
}

void writeEEPROM()
{
  byte value_LMP = lamp.out;
  byte value_FAN = fan.out;
  byte value_PWM = int(PWM_OFFSET*100.0F);
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_LMP);
  Sprintln(value_FAN);
  Sprintln(value_PWM);

  EEPROM.update(address_LMP, value_LMP);
  EEPROM.update(address_FAN, value_FAN);
  EEPROM.update(address_PWM, value_PWM);

  readEEPROM();
}
