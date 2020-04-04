// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
  //local_delay(5);
  readEEPROM();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  configStateOn       = EEPROM.read(address_state)==1 ? true : false; 
  configFanMode       = EEPROM.read(address_fan); 
  configSwingMode     = EEPROM.read(address_swing)==1 ? true : false;
  configRunMode       = EEPROM.read(address_run);
  configTempSetpoint  = EEPROM.read(address_temp);

  if (configStateOn > 1)
    configStateOn = false;

  if (configFanMode < FANMODES_MIN || configFanMode > FANMODES_MAX)
    configFanMode = FANMODES_DEF;

  if (configSwingMode > 1)
    configSwingMode = false;

  if (configRunMode < RUNMODES_MIN || configRunMode > RUNMODES_MAX)
    configRunMode = RUNMODES_DEF;

  if (configTempSetpoint < minTemp || configTempSetpoint > maxTemp)
    configTempSetpoint = defTemp;

  Sprint("Read EEPROM=> State: ");
  Sprint(configStateOn ? mqtt_cmdOn : mqtt_cmdOff);
  Sprint(" / Swing: ");
  Sprint(configSwingMode ? mqtt_cmdOn : mqtt_cmdOff);
  Sprint(" / Fan: ");
  Sprint(fanModes[configFanMode]);
  Sprint(" / Run: ");
  Sprint(runModes[configRunMode]);
  Sprint(" / Temperature: ");
  Sprint(configTempSetpoint);
  Sprintln("C");
}

void writeEEPROM()
{
  byte value_State = configStateOn ? 1 : 0;
  byte value_Fan   = configFanMode;
  byte value_Swing = configSwingMode ? 1 : 0;
  byte value_Run   = configRunMode;
  byte value_Temp  = configTempSetpoint;
  
  EEPROM.put(address_state, value_State);
  EEPROM.put(address_fan, value_Fan);
  EEPROM.put(address_swing, value_Swing);
  EEPROM.put(address_run, value_Run);
  EEPROM.put(address_temp, value_Temp);

  EEPROM.commit();

  Sprint("Write EEPROM=> State: ");
  Sprint(value_State==1 ? mqtt_cmdOn : mqtt_cmdOff);
  Sprint(" / Swing: ");
  Sprint(value_Swing==1 ? mqtt_cmdOn : mqtt_cmdOff);
  Sprint(" / Fan: ");
  Sprint(fanModes[value_Fan]);
  Sprint(" / Run: ");
  Sprint(runModes[value_Run]);
  Sprint(" / Temperature: ");
  Sprint(value_Temp);
  Sprintln("C");

  readEEPROM();
}
