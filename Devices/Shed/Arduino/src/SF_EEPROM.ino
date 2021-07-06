// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  readEEPROM();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_LGT = EEPROM.read(address_LGT);

  toggleSw[0].state = value_LGT==1;

  Sprintln("Read EEPROM: ");
  Sprintln(toggleSw[0].state);
}

void writeEEPROM()
{
  byte value_LGT = toggleSw[0].state ? 1 : 0;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_LGT);

  EEPROM.update(address_LGT, value_LGT);

  readEEPROM();
}
