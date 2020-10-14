// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  EEPROM.begin(EEPROM_SIZE);
  local_delay(5);
  readEEPROM();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_PWH  = EEPROM.read(address_PWH);
  byte value_PWL  = EEPROM.read(address_PWL);
  byte value_LED  = EEPROM.read(address_LED);
  byte value_LCK  = EEPROM.read(address_LCK);
  byte value_SPR  = EEPROM.read(address_SPR);

  passcode = value_PWH * 256 + value_PWL;
  if (passcode == 0)
    passcode = DEFAULT_PASSCODE;  //DEFAULT passcode
    
  ledDelayOff = value_LED;
  logDelayOff = value_LCK;
  sprDelayOff = value_SPR;
    
  Sprintln("Read EEPROM: ");
  Sprintln(passcode);
  Sprintln(ledDelayOff);
  Sprintln(logDelayOff);
  Sprintln(sprDelayOff);
}

void writeEEPROM()
{
  if (passcode == 0)
    passcode = DEFAULT_PASSCODE;  //DEFAULT passcode
  
  byte value_PWH  = highByte(passcode);
  byte value_PWL  = lowByte(passcode);
  byte value_LED  = ledDelayOff;
  byte value_LCK  = logDelayOff;
  byte value_SPR  = sprDelayOff;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_PWH);
  Sprintln(value_PWL);
  Sprintln(value_LED);
  Sprintln(value_LCK);
  Sprintln(value_SPR);

  EEPROM.put(address_PWH, value_PWH);
  EEPROM.put(address_PWL, value_PWL);
  EEPROM.put(address_LED, value_LED);
  EEPROM.put(address_LCK, value_LCK);
  EEPROM.put(address_SPR, value_SPR);

  EEPROM.commit();

  readEEPROM();
}
