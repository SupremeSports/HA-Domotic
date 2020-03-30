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
  waterHeaterOn = EEPROM.read(address_state)==1 ? true : false; 

  if (waterHeaterOn > 1)
    waterHeaterOn = false;

  Sprint("Read EEPROM=> State: ");
  Sprintln(waterHeaterOn ? mqtt_cmdOn : mqtt_cmdOff);
}

void writeEEPROM()
{
  byte value_State = waterHeaterOn ? 1 : 0;
  
  EEPROM.put(address_state, value_State);

  EEPROM.commit();

  Sprint("Write EEPROM=> State: ");
  Sprintln(value_State==1 ? mqtt_cmdOn : mqtt_cmdOff);

  readEEPROM();
}
