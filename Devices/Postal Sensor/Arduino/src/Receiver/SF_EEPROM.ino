// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  EEPROM.begin(EEPROM_SIZE);
  //local_delay(5);
  readEEPROM();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte value_IDH  = EEPROM.read(address_IDH); 
  byte value_IDL  = EEPROM.read(address_IDL); 
  byte value_Stts = EEPROM.read(address_Stts);
  byte value_VccH = EEPROM.read(address_VccH);
  byte value_VccL = EEPROM.read(address_VccL);
  byte value_Rssi = EEPROM.read(address_Rssi);
  byte value_Frcd = EEPROM.read(address_Frcd);
  byte value_Dlay = EEPROM.read(address_Dlay);

  int value_ID = value_IDH * 256 + value_IDL;
  
  Id = String(value_ID);
  mailPresent = (value_Stts == 1) ? true : false;
  Vcc = String(float(value_VccH) + (float(value_VccL)/100.0));
  Bat = convertBat(Vcc);
  Rssi = "-" + String(value_Rssi);
  newStatusForced = value_Frcd==0;
  enableFiveMinuteDelay = value_Dlay==0;
  
  Sprintln("Read EEPROM: ");
  Sprintln(Id);
  Sprintln(mailPresent);
  Sprintln(Vcc);
  Sprintln(Bat);
  Sprintln(Rssi);
  Sprintln(newStatusForced ? json_Forced : json_Sender);
  Sprintln(enableFiveMinuteDelay ? mqtt_cmdOn : mqtt_cmdOff);
}

void writeEEPROM()
{
  byte value_IDH  = highByte(Id.toInt());
  byte value_IDL  = lowByte(Id.toInt());
  byte value_Stts = mailPresent ? 1 : 0;
  byte value_VccH = int(Vcc.toFloat());
  byte value_VccL = String((Vcc.toFloat()-value_VccH)*100.0, 0).toInt();
  byte value_Rssi = abs(int(Rssi.toFloat()));
  byte value_Frcd = newStatusForced ? 0 : 1;
  byte value_Dlay = enableFiveMinuteDelay ? 0 : 1;
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_IDH);
  Sprintln(value_IDL);
  Sprintln(value_Stts);
  Sprintln(value_VccH);
  Sprintln(value_VccL);
  Sprintln(value_Rssi);
  Sprintln(value_Frcd);
  Sprintln(value_Dlay);
  
  EEPROM.put(address_IDH, value_IDH);
  EEPROM.put(address_IDL, value_IDL);
  EEPROM.put(address_Stts, value_Stts);
  EEPROM.put(address_VccH, value_VccH);
  EEPROM.put(address_VccL, value_VccL);
  EEPROM.put(address_Rssi, value_Rssi);
  EEPROM.put(address_Frcd, value_Frcd);
  EEPROM.put(address_Dlay, value_Dlay);

  EEPROM.commit();

  readEEPROM();
}
