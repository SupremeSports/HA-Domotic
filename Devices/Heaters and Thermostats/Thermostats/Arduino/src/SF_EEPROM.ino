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
  byte value_DIM  = EEPROM.read(address_DIM);
  byte value_BIP  = EEPROM.read(address_BIP);
  byte value_VOL  = EEPROM.read(address_VOL);

  passcode = value_PWH * 256 + value_PWL;

  if (passcode == 0 || passcode == 65535)
    passcode = DEFAULT_PASSCODE;
    
  ledDelayOff = value_LED;
  logDelayOff = value_LCK;
  screenDimValue = constrain(value_DIM, 0, 7);
  beepON = value_BIP==1;
  beepVolume = constrain(value_VOL, 0, 7);

  Sprintln("Read EEPROM: ");
  Sprintln(passcode);
  Sprintln(ledDelayOff);
  Sprintln(logDelayOff);
  Sprintln(screenDimValue);
  Sprintln(beepON);
  Sprintln(beepVolume);
}

void writeEEPROM()
{
  if (passcode == 0 || passcode == 65535)
    passcode = DEFAULT_PASSCODE;  //DEFAULT passcode
  
  byte value_PWH  = highByte(passcode);
  byte value_PWL  = lowByte(passcode);
  byte value_LED  = ledDelayOff;
  byte value_LCK  = logDelayOff;
  byte value_DIM  = screenDimValue;
  byte value_BIP  = beepON ? 1 : 0;
  byte value_VOL  = beepVolume;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_PWH);
  Sprintln(value_PWL);
  Sprintln(value_LED);
  Sprintln(value_LCK);
  Sprintln(value_DIM);
  Sprintln(value_BIP);
  Sprintln(value_VOL);

  EEPROM.put(address_PWH, value_PWH);
  EEPROM.put(address_PWL, value_PWL);
  EEPROM.put(address_LED, value_LED);
  EEPROM.put(address_LCK, value_LCK);
  EEPROM.put(address_DIM, value_DIM);
  EEPROM.put(address_BIP, value_BIP);
  EEPROM.put(address_VOL, value_VOL);

  EEPROM.commit();

  readEEPROM();
}
