// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- PROCESS COMMAND JSON ---------------------------------------
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

  bool ok = false;

  if (root.containsKey(json_reset))
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(root[json_reset], mqtt_cmdOn) != 0)
      return false;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);
    ESP.restart();
  }

  if (root.containsKey(json_waterTemp))
  {
    if (strcmp(root[json_waterTemp], json_unavailable) != 0 && strcmp(root[json_waterTemp], json_blank) != 0)
      getWaterTemp(root[json_waterTemp]);
    else
      getWaterTemp(String(initValue));
  }
  
  if (root.containsKey(json_waterPH))
  {
    if (strcmp(root[json_waterPH], json_unavailable) != 0 && strcmp(root[json_waterPH], json_blank) != 0)
      getWaterPH(root[json_waterPH]);
    else
      getWaterPH(String(initValue));
  }
  
  if (root.containsKey(json_outTemp))
  {
    if (strcmp(root[json_outTemp], json_unavailable) != 0 && strcmp(root[json_outTemp], json_blank) != 0)
      getOutTemp(root[json_outTemp]);
    else
      getOutTemp(String(initValue));
  }
  
  if (root.containsKey(json_outHum))
  {
    if (strcmp(root[json_outHum], json_unavailable) != 0 && strcmp(root[json_outHum], json_blank) != 0)
      getOutHum(root[json_outHum]);
    else
      getOutHum(String(initValue));
  }
  
  if (root.containsKey(json_timeText))
  {
    timeTextStringBuffer = padString(root[json_timeText], timeDigits);
    rollingTimeTextIndex = 1;
  }
  
  if (root.containsKey(json_tempText))
  {
    tempTextStringBuffer = padString(root[json_tempText], tempDigits);
    rollingTempTextIndex = 1;
  }

  if (root.containsKey(json_scroll))
  {
    configScrollSpeed = map(root[json_scroll], 0, 100, 1000, 0);

    Sprint("Scroll: ");
    Sprintln(configScrollSpeed);

    ok = true;
  }

  return ok;
}

void sendCommandState()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_scroll] = map(configScrollSpeed, 1000, 0, 0, 100);

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_controlStts, buffer);//, true);
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

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
      stateOn = true;
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
      stateOn = false;

    Sprint("State: ");
    Sprintln(stateOn ? mqtt_cmdOn : mqtt_cmdOff);
  }

  if (root.containsKey(json_transition))
  {
    configCycleSpeed = root[json_transition];

    Sprint("Transition: ");
    Sprintln(configCycleSpeed);
  }

  if (root.containsKey(json_effect) && !initDisplays)
  {
    colorAutoSwitch = false;
    displayFeature = 0;
    changeColor = false;
    
    for (int i = EFFECT_MIN; i <= EFFECT_MAX; i++)
    {
      if (strcmp(root[json_effect], effects[i]) == 0)
      {
        displayFeature = i;
        local_delay(10);
        break;
      }
    }

    //AllEffectsRolling
    if (displayFeature == EFFECT_MAX) 
    {
      colorAutoSwitch = true;
      displayFeature = 1;
      rainbowIndex = 0;
    }

    Sprint("Display feature: ");
    Sprintln(displayFeature);
  }
  
  if (root.containsKey(json_brightness))
  {
    brightness = root[json_brightness];
    Sprint("Brightness: ");
    Sprintln(brightness);
  }
  
  if (root.containsKey(json_color))
  {
    colorAutoSwitch = false;
    changeColor = true;
    displayFeature = 0;
    
    configRedCnl = root[json_color]["r"];
    configGreenCnl = root[json_color]["g"];
    configBlueCnl = root[json_color]["b"];

    Sprint("RGB: ");
    Sprint(configRedCnl);
    Sprint(", ");
    Sprint(configGreenCnl);
    Sprint(", ");
    Sprintln(configBlueCnl);
  }

  return true;
}

void sendLightColorsState()
{
  //Force color to fit with HA bulb color
  byte r = (!changeColor || displayFeature != 0 || colorAutoSwitch) ? 251 : configRedCnl;
  byte g = (!changeColor || displayFeature != 0 || colorAutoSwitch) ? 205 : configGreenCnl;
  byte b = (!changeColor || displayFeature != 0 || colorAutoSwitch) ? 65 : configBlueCnl;

  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = stateOn ? mqtt_cmdOn : mqtt_cmdOff;
  
  JsonObject& color = root.createNestedObject(json_color);
  color["r"] = r;
  color["g"] = g;
  color["b"] = b;

  root[json_brightness] = brightness;

  if (displayFeature == 0 || changeColor)
    root[json_effect] = "null";//effects[EFFECT_MIN];   //"Default"
  else if (colorAutoSwitch)
    root[json_effect] = effects[EFFECT_MAX];   //AllEffectsRolling
  else
    root[json_effect] = effects[displayFeature];

  root[json_transition] = configCycleSpeed;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_ledStts, buffer);//, true);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
  root[json_rssi] = result;

  dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
  root[json_rssiPercent] = result;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
}
