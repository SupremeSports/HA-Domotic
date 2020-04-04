// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processCommandJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return false;
  }

  bool ok = true;

  if (root.containsKey(json_reset))
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(root[json_reset], mqtt_cmdOn) != 0)
      return true;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);
    ESP.restart();
  }

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
    {
      if (configStateOn)
        ok = false;
      configStateOn = true;
    }
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
    {
      if (!configStateOn)
        ok = false;
      configStateOn = false;
    }

    setOnOffStates();

    sendDataToI2C(I2C_SET_OPTION_BLANK_MODE, configBlankMode);
    sendDataToI2C(I2C_SET_OPTION_DAY_BLANKING, configDayBlanking);
  }
  else if (root.containsKey(json_hourMode))
    sendDataToI2C(I2C_SET_OPTION_12_24, (strcmp(root[json_hourMode], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_blankLead))
    sendDataToI2C(I2C_SET_OPTION_BLANK_LEAD, (strcmp(root[json_blankLead], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_scrollback))
    sendDataToI2C(I2C_SET_OPTION_SCROLLBACK, (strcmp(root[json_scrollback], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_suppressACP))
    sendDataToI2C(I2C_SET_OPTION_SUPPRESS_ACP, (strcmp(root[json_suppressACP], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_useFade))
    sendDataToI2C(I2C_SET_OPTION_FADE, (strcmp(root[json_useFade], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_useLDR))
    sendDataToI2C(I2C_SET_OPTION_USE_LDR, (strcmp(root[json_useLDR], mqtt_cmdOn) == 0) ? 0 : 1);
  else if (root.containsKey(json_slotsMode))
    sendDataToI2C(I2C_SET_OPTION_SLOTS_MODE, (strcmp(root[json_slotsMode], mqtt_cmdOn) == 0) ? 1 : 0); //Reversed
    
  //Integers
  else
  {
    uint16_t intValue = 0;
    uint16_t intCmd   = 0;
    if (root.containsKey(json_dateFormat))
    {
      intValue = atoi(root[json_dateFormat]);
      if (intValue < DATE_FORMAT_MIN and intValue > DATE_FORMAT_MAX)
        intValue = DATE_FORMAT_DEFAULT;

      intCmd = I2C_SET_OPTION_DATE_FORMAT;
    }
    else if (root.containsKey(json_dayBlanking))
    {
      intValue = atoi(root[json_dayBlanking]);
      if (intValue < DAY_BLANKING_MIN and intValue > DAY_BLANKING_MAX)
        intValue = DAY_BLANKING_DEFAULT;

      intCmd = I2C_SET_OPTION_DAY_BLANKING;
    }
    else if (root.containsKey(json_blankFrom))
    {
      intValue = atoi(root[json_blankFrom]);
      if (intValue < BLANK_FROM_MIN and intValue > BLANK_FROM_MAX)
        intValue = BLANK_FROM_DEFAULT;

      intCmd = I2C_SET_OPTION_BLANK_START;
    }
    else if (root.containsKey(json_blankTo))
    {
      intValue = atoi(root[json_blankTo]);
      if (intValue < BLANK_TO_MIN and intValue > BLANK_TO_MAX)
        intValue = BLANK_TO_DEFAULT;

      intCmd = I2C_SET_OPTION_BLANK_END;
    }
    else if (root.containsKey(json_fadeSteps))
    {
      intValue = atoi(root[json_fadeSteps]);
      if (intValue < FADE_STEPS_MIN and intValue > FADE_STEPS_MAX)
        intValue = FADE_STEPS_DEFAULT;

      intCmd = I2C_SET_OPTION_FADE_STEPS;
    }
    else if (root.containsKey(json_scrollSteps))
    {
      intValue = atoi(root[json_scrollSteps]);
      if (intValue < SCROLL_STEPS_MIN and intValue > SCROLL_STEPS_MAX)
        intValue = SCROLL_STEPS_DEFAULT;

      intCmd = I2C_SET_OPTION_SCROLL_STEPS;
    }
    else if (root.containsKey(json_blankMode))
    {
      intValue = atoi(root[json_blankMode]);
      if (intValue < BLANK_MODE_MIN and intValue > BLANK_MODE_MAX)
        intValue = BLANK_MODE_DEFAULT;

      intCmd = I2C_SET_OPTION_BLANK_MODE;
    }
    else if (root.containsKey(json_minDim))
    {
      intValue = atoi(root[json_minDim]);
      if (intValue < MIN_DIM_MIN and intValue > MIN_DIM_MAX)
        intValue = MIN_DIM_DEFAULT;

      intCmd = I2C_SET_OPTION_MIN_DIM;
    }   
    else
    {
      Sprint("Invalid Command!");
      return false;
    }

    sendDataToI2C(intCmd, intValue);
  }

  return ok;
}

void setOnOffStates()
{
  if (!configStateOn && configBacklightOn)
    configBlankMode = 0;      //Blank tubes only
  else if (configStateOn && !configBacklightOn)
    configBlankMode = 1;      //Blank LEDs only
  else
    configBlankMode = 2;      //Blank ALL

  if (configStateOn && configBacklightOn)
    configDayBlanking = 0;    //Don't blank
  else
    configDayBlanking = 3;    //Always blank
}

void getOnOffStates()
{
  if (configDayBlanking == 0) //Don't blank
  {
    configBacklightOn = true;
    configStateOn = true;
  }
  else if (configDayBlanking == 3) //Always blank
  {
    configBacklightOn = false;
    configStateOn = false;
    
    if (configBlankMode == 0) //Blank tubes only
      configBacklightOn = true;
    if (configBlankMode == 1) //Blank LEDs only
      configStateOn = true;
  }
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS LED JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processLightColorsJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return false;
  }

  bool ok = true;

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
    {
      if (configBacklightOn)
        ok = false;
      configBacklightOn = true;
    }
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
    {
      if (!configBacklightOn)
        ok = false;
      configBacklightOn = false;
    }

    setOnOffStates();

    sendDataToI2C(I2C_SET_OPTION_BLANK_MODE, configBlankMode);
    sendDataToI2C(I2C_SET_OPTION_DAY_BLANKING, configDayBlanking);
  }

  if (root.containsKey(json_effect))
  {
    for (int i = BACKLIGHT_MIN; i <= BACKLIGHT_MAX; i++)
    {
      if (strcmp(root[json_effect], effects[i]) == 0)
      {
        configBacklightMode = i;
        sendDataToI2C(I2C_SET_OPTION_BACKLIGHT_MODE, configBacklightMode);
        break;
      }
    }
    ok = true;
  }
  
  if (root.containsKey(json_color))
  {
    byte r = root[json_color]["r"];
    byte g = root[json_color]["g"];
    byte b = root[json_color]["b"];
    
    configRedCnl = constrain(map(r, 0, 255, 0, 15), 0, 15);
    configGreenCnl = constrain(map(g, 0, 255, 0, 15), 0, 15);
    configBlueCnl = constrain(map(b, 0, 255, 0, 15), 0, 15);

    sendDataToI2C(I2C_SET_OPTION_RED_CHANNEL, configRedCnl);
    sendDataToI2C(I2C_SET_OPTION_GREEN_CHANNEL, configGreenCnl);
    sendDataToI2C(I2C_SET_OPTION_BLUE_CHANNEL, configBlueCnl);

    ok = true;
  }

  if (root.containsKey(json_speed))
  {
    configCycleSpeed = constrain(root[json_speed], 4, 64);
 
    sendDataToI2C(I2C_SET_OPTION_CYCLE_SPEED, configCycleSpeed);

    ok = true;
  }

  return ok;
}

void sendLightColorsState()
{
  byte r = 0;
  byte g = 0;
  byte b = 0;

  //Force color to fit with HA bulb color
  if (configBacklightMode == 2 || configBacklightMode == 5)
  {
    r = 251;
    g = 205;
    b = 65;
  }
  else
  {
    r = constrain(map(configRedCnl, 0, 15, 0, 255), 0, 255);
    g = constrain(map(configGreenCnl, 0, 15, 0, 255), 0, 255);
    b = constrain(map(configBlueCnl, 0, 15, 0, 255), 0, 255);
  }
  
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = configBacklightOn ? mqtt_cmdOn : mqtt_cmdOff;

  JsonObject& color = root.createNestedObject(json_color);
  color["r"] = r;
  color["g"] = g;
  color["b"] = b;

  root[json_effect] = effects[configBacklightMode];

  root[json_speed] = configCycleSpeed;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_ledStts, buffer);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendNixieStates()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  //State
  root[json_state] = configStateOn ? mqtt_cmdOn : mqtt_cmdOff;
  //Booleans
  root[json_hourMode] = configHourMode==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_blankLead] = configBlankLead==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_scrollback] = configScrollback==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_suppressACP] = configSuppressACP==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_useFade] = configUseFade==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_useLDR] = configUseLDR==1 ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_slotsMode] = configSlotsMode==1 ? mqtt_cmdOn : mqtt_cmdOff;
  
  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_controlStts, buffer);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = configStateOn ? mqtt_cmdOn : mqtt_cmdOff;

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  dtostrf(caseTemp, 6, 2, result); // Leave room for too large numbers!
  root[json_casetemp] = result;

  dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
  root[json_rssi] = result;

  dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
  root[json_rssiPercent] = result;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_clockJson, buffer);
}
