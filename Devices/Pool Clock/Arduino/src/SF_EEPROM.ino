// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  EEPROM.begin(EEPROM_SIZE);
  //local_delay(5);
  //readEEPROM(); //Wait to let time to display initial show
}
  
// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_Sts = EEPROM.read(address_Sts);
  byte value_CAS = EEPROM.read(address_CAS);
  byte value_CCr = EEPROM.read(address_CCr);
  byte value_Spd = EEPROM.read(address_Spd);
  byte value_Opt = EEPROM.read(address_Opt);
  byte value_Lum = EEPROM.read(address_Lum);
  byte value_Red = EEPROM.read(address_Red);
  byte value_Grn = EEPROM.read(address_Grn);
  byte value_Blu = EEPROM.read(address_Blu);

  stateOn = (value_Sts == 1) ? true : false;
  colorAutoSwitch = (value_CAS == 1) ? true : false;
  changeColor = (value_CCr == 1) ? true : false;
  configCycleSpeed = map(value_Spd, 0, 255, 0, 1000);
  displayFeature = value_Opt;
  brightness = value_Lum;
  configRedCnl = value_Red;
  configGreenCnl = value_Grn;
  configBlueCnl = value_Blu;
  
  Sprintln("Read EEPROM: ");
  Sprintln(stateOn);
  Sprintln(colorAutoSwitch);
  Sprintln(changeColor);
  Sprintln(configCycleSpeed);
  Sprintln(displayFeature);
  Sprintln(brightness);
  Sprintln(configRedCnl);
  Sprintln(configGreenCnl);
  Sprintln(configBlueCnl);
}

void writeEEPROM()
{
  byte value_Sts = stateOn ? 1 : 0;
  byte value_CAS = colorAutoSwitch ? 1 : 0;
  byte value_CCr = changeColor ? 1 : 0;
  byte value_Spd = map(configCycleSpeed, 0, 1000, 0, 255);
  byte value_Opt = displayFeature;
  byte value_Lum = brightness;
  byte value_Red = configRedCnl;
  byte value_Grn = configGreenCnl;
  byte value_Blu = configBlueCnl;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_Sts);
  Sprintln(value_CAS);
  Sprintln(value_CCr);
  Sprintln(value_Spd);
  Sprintln(value_Opt);
  Sprintln(value_Lum);
  Sprintln(value_Red);
  Sprintln(value_Grn);
  Sprintln(value_Blu);
  
  EEPROM.put(address_Sts, value_Sts);
  EEPROM.put(address_CAS, value_CAS);
  EEPROM.put(address_CCr, value_CCr);
  EEPROM.put(address_Spd, value_Spd);
  EEPROM.put(address_Opt, value_Opt);
  EEPROM.put(address_Lum, value_Lum);
  EEPROM.put(address_Red, value_Red);
  EEPROM.put(address_Grn, value_Grn);
  EEPROM.put(address_Blu, value_Blu);

  EEPROM.commit();

  readEEPROM();
}
