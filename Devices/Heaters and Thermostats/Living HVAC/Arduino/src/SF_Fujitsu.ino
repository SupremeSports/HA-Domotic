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
  configStateOn = (state ? true : false);
  ac.setCmd(configStateOn ? kFujitsuAcCmdTurnOn : kFujitsuAcCmdTurnOff);

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

  if (!configStateOn && configRunMode >= RUNMODES_MIN && configRunMode <= RUNMODES_MAX)
  {
    setStateMQTTbool(true);
    local_delay(1000);
  }
  
  switch(configRunMode)
  {
    case 0:       //Off
      setStateMQTTbool(false);
      break;
    case 1:       //Auto
      ac.setMode(kFujitsuAcModeAuto);
      break;
    case 2:       //Cool
      ac.setMode(kFujitsuAcModeCool);
      break;
    case 3:       //Dry
      ac.setMode(kFujitsuAcModeDry);
      break;
    case 4:       //Fan
      ac.setMode(kFujitsuAcModeFan);
      break;
    case 5:       //Heat
      ac.setMode(kFujitsuAcModeHeat);
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

  if (!configStateOn && configFanMode >= FANMODES_MIN && configFanMode <= FANMODES_MAX)
  {
    setStateMQTTbool(true);
    local_delay(1000);
  }
  
  switch(configFanMode)
  {
    case 0:       //Auto
      ac.setFanSpeed(kFujitsuAcFanAuto);
      break;
    case 1:       //High
      ac.setFanSpeed(kFujitsuAcFanHigh);
      break;
    case 2:       //Medium
      ac.setFanSpeed(kFujitsuAcFanMed);
      break;
    case 3:       //Low
      ac.setFanSpeed(kFujitsuAcFanLow);
      break;
    case 4:       //Quiet
      ac.setFanSpeed(kFujitsuAcFanQuiet);
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
  /*if (!configStateOn)
  {
    setStateMQTTbool(true);
    local_delay(1000);
  }*/

  bool state = false;
  if (strcmp(message,"On")==0)
  {
    state = true;
    //setStateMQTTbool(true);
  }
  else if (strcmp(message,"Off")==0)
    state = false;
  else
    return;

  /*if (!configStateOn && state)
  {
    setStateMQTTbool(true);
    local_delay(1000);
  }*/
    
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
