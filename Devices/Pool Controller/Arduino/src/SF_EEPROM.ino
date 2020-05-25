// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  EEPROM.begin();
  local_delay(5);
  readEEPROM(); //Wait to let time to display initial show
}
  
// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_flowDrain = EEPROM.read(address_Drn);
  byte value_flowSlide = EEPROM.read(address_Sld);
  byte value_flowHeatp = EEPROM.read(address_Htp);

  flowSkimDrain = value_flowDrain;
  flowSlide = value_flowSlide;
  flowHeater = value_flowHeatp;
  
  Sprintln("Read EEPROM: ");
  Sprintln(flowSkimDrain);
  Sprintln(flowSlide);
  Sprintln(flowHeater);
}

void writeEEPROM()
{
  byte value_flowDrain = flowSkimDrain;
  byte value_flowSlide = flowSlide;
  byte value_flowHeatp = flowHeater;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_flowDrain);
  Sprintln(value_flowSlide);
  Sprintln(value_flowHeatp);
  
  EEPROM.update(address_Drn, value_flowDrain);
  EEPROM.update(address_Sld, value_flowSlide);
  EEPROM.update(address_Htp, value_flowHeatp);

  readEEPROM();
}
