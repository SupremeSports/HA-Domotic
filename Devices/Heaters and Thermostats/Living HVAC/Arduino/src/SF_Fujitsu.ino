// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- FUJITSU IR SETUP -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void initFujitsuIR()
{
  Sprintln("Init AC Fujitsu...");
  ac.begin();
  local_delay(200);

  readEEPROM();

  ac.setModel(MODEL);
  
  char tempChar[5];
  itoa(configTempSetpoint, tempChar, 4);

  setSwingMQTT(configSwingMode==1 ? (char*)"On" : (char*)"Off");
  setModeMQTT((char*)runModes[configRunMode]);
  setFanMQTT((char*)fanModes[configFanMode]);
  setTempMQTT(tempChar);
  setStateMQTTbool(configStateOn ? true : false);

  sendDataToHvac = true;
}

//Integrated POWER ON/OFF into other commands
/*void setStateMQTT(char* message)
{
  if (strcmp(message,mqtt_cmdOn)==0)
    setStateMQTTbool(true);
  else if (strcmp(message,mqtt_cmdOff)==0)
    setStateMQTTbool(false);
}*/

void setStateMQTTbool(bool state)
{
  ac.setCmd(state ? kFujitsuAcCmdTurnOn : kFujitsuAcCmdTurnOff);
  configStateOn = (state ? true : false);

  Sprint("State set to: ");
  Sprintln(state);
}

void setModeMQTT(char* message)
{
  for (int i=RUNMODES_MIN; i<=RUNMODES_MAX; i++)
  {
    if (strcmp(message,runModes[i])==0)
    {
      configRunMode = i;
      break;
    }
  }
  
  switch(configRunMode)
  {
    case 0:       //Off
      setStateMQTTbool(false);
      break;
    case 1:       //Auto
      ac.setMode(kFujitsuAcModeAuto);
      setStateMQTTbool(true);
      break;
    case 2:       //Cool
      ac.setMode(kFujitsuAcModeCool);
      setStateMQTTbool(true);
      break;
    case 3:       //Dry
      ac.setMode(kFujitsuAcModeDry);
      setStateMQTTbool(true);
      break;
    case 4:       //Fan
      ac.setMode(kFujitsuAcModeFan);
      setStateMQTTbool(true);
      break;
    case 5:       //Heat
      ac.setMode(kFujitsuAcModeHeat);
      setStateMQTTbool(true);
      break;
  }

  Sprint("Mode set to: ");
  Sprintln(runModes[configRunMode]);
}

void setFanMQTT(char* message)
{
  for (int i=FANMODES_MIN; i<=FANMODES_MAX; i++)
  {
    if (strcmp(message,fanModes[i])==0)
    {
      configFanMode = i;
      break;
    }
  }
  
  switch(configFanMode)
  {
    case 0:       //Auto
      ac.setFanSpeed(kFujitsuAcFanAuto);
      setStateMQTTbool(true);
      break;
    case 1:       //High
      ac.setFanSpeed(kFujitsuAcFanHigh);
      setStateMQTTbool(true);
      break;
    case 2:       //Medium
      ac.setFanSpeed(kFujitsuAcFanMed);
      setStateMQTTbool(true);
      break;
    case 3:       //Low
      ac.setFanSpeed(kFujitsuAcFanLow);
      setStateMQTTbool(true);
      break;
    case 4:       //Quiet
      ac.setFanSpeed(kFujitsuAcFanQuiet);
      setStateMQTTbool(true);
      break;
  }

  Sprint("Fan set to: ");
  Sprintln(fanModes[configFanMode]);
}

void setTempMQTT(char* message)
{
  uint8_t state = atoi(message);
  state = constrain(state, minTemp, maxTemp);
  ac.setTemp(state);

  configTempSetpoint = state;

  Sprint("Temp set to: ");
  Sprint(state);
  Sprintln("C");
}

void setSwingMQTT(char* message)
{
  bool state = false;
  if (strcmp(message,"On")==0)
  {
    state = true;
    setStateMQTTbool(true);
  }
  else if (strcmp(message,"Off")==0)
    state = false;
  else
    return;
    
  ac.setSwing(state ? kFujitsuAcSwingVert : kFujitsuAcSwingOff);
  configSwingMode = (state ? true : false);

  Sprint("Swing set to: ");
  Sprintln(state);
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- FUJITSU IR FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendIRsignal()
{
  Sprintln("Sending IR command to A/C ...");
  #if SEND_FUJITSU_AC
    ac.send();
  #else
    Sprintln("Can't send because SEND_FUJITSU_AC has been disabled.");
  #endif

  printState();
  local_delay(50);
  writeEEPROM();
}

void printState()
{
  // Display the settings.
  Sprintln("Fujitsu A/C remote is in the following state:");
  Sprintf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Sprint("  IR Code: 0x");
  for (uint8_t i = 0; i < ac.getStateLength(); i++)
    Sprintf("%02X", ir_code[i]);
  Sprintln();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
  //local_delay(5);
  readEEPROM();
}

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
  Sprint(configStateOn ? "ON" : "OFF");
  Sprint(" / Swing: ");
  Sprint(configSwingMode ? "ON" : "OFF");
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
  Sprint(value_State==1 ? "ON" : "OFF");
  Sprint(" / Swing: ");
  Sprint(value_Swing==1 ? "ON" : "OFF");
  Sprint(" / Fan: ");
  Sprint(fanModes[value_Fan]);
  Sprint(" / Run: ");
  Sprint(runModes[value_Run]);
  Sprint(" / Temperature: ");
  Sprint(value_Temp);
  Sprintln("C");

  readEEPROM();
}
