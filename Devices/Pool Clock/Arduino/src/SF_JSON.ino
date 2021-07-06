// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- PROCESS COMMAND JSON ---------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processCommandJson(char* message)
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Sprintln(json_parseFailed);
    return false;
  }

  bool ok = false;

  if (doc[json_reset])
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(doc[json_reset], mqtt_cmdOn) != 0)
      return false;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);
    ESP.restart();
  }

  if (doc[json_waterTemp])
  {
    if (isValidNumber(doc[json_waterTemp]))
      getWaterTemp(doc[json_waterTemp]);
    else
      getWaterTemp(String(initValue));
  }
  
  if (doc[json_waterPH])
  {
    if (isValidNumber(doc[json_waterPH]))
      getWaterPH(doc[json_waterPH]);
    else
      getWaterPH(String(initValue));
  }
  
  if (doc[json_outTemp])
  {
    if (isValidNumber(doc[json_outTemp]))
      getOutTemp(doc[json_outTemp]);
    else
      getOutTemp(String(initValue));
  }
  
  if (doc[json_outHum])
  {
    if (isValidNumber(doc[json_outHum]))
      getOutHum(doc[json_outHum]);
    else
      getOutHum(String(initValue));
  }
  
  if (doc[json_timeText])
  {
    timeTextStringBuffer = padString(doc[json_timeText], timeDigits);
    rollingTimeTextIndex = 1;
  }
  
  if (doc[json_tempText])
  {
    tempTextStringBuffer = padString(doc[json_tempText], tempDigits);
    rollingTempTextIndex = 1;
  }

  if (doc[json_scroll])
  {
    configScrollSpeed = map(doc[json_scroll], 0, 100, 1000, 0);

    Sprint("Scroll: ");
    Sprintln(configScrollSpeed);

    ok = true;
  }

  return ok;
}

void sendCommandState()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  root[json_scroll] = map(configScrollSpeed, 1000, 0, 0, 100);

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_controlStts, buffer);//, true);
  mqttClient.loop();
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS LED JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processLightColorsJson(char* message)
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Sprintln(json_parseFailed);
    return false;
  }

  if (doc[json_state])
  {
    if (strcmp(doc[json_state], mqtt_cmdOn) == 0)
      stateOn = true;
    else if (strcmp(doc[json_state], mqtt_cmdOff) == 0)
      stateOn = false;

    Sprint("State: ");
    Sprintln(stateOn ? mqtt_cmdOn : mqtt_cmdOff);
  }

  if (doc[json_transition])
  {
    configCycleSpeed = doc[json_transition];

    Sprint("Transition: ");
    Sprintln(configCycleSpeed);
  }

  if (doc[json_effect] && !initDisplays)
  {
    colorAutoSwitch = false;
    displayFeature = 0;
    changeColor = false;
    
    for (int i = EFFECT_MIN; i <= EFFECT_MAX; i++)
    {
      if (strcmp(doc[json_effect], effects[i]) == 0)
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
  
  if (doc[json_brightness])
  {
    brightness = doc[json_brightness];
    Sprint("Brightness: ");
    Sprintln(brightness);
  }
  
  if (doc[json_color])
  {
    colorAutoSwitch = false;
    changeColor = true;
    displayFeature = 0;
    
    configRedCnl = doc[json_color]["r"];
    configGreenCnl = doc[json_color]["g"];
    configBlueCnl = doc[json_color]["b"];

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

  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  root[json_state] = stateOn ? mqtt_cmdOn : mqtt_cmdOff;
  
  JsonObject color = doc.createNestedObject(json_color);
  color["r"] = r;
  color["g"] = g;
  color["b"] = b;

  doc[json_brightness] = brightness;

  if (displayFeature == 0 || changeColor)
    root[json_effect] = "null";//effects[EFFECT_MIN];   //"Default"
  else if (colorAutoSwitch)
    root[json_effect] = effects[EFFECT_MAX];   //AllEffectsRolling
  else
    root[json_effect] = effects[displayFeature];

  root[json_transition] = configCycleSpeed;

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_ledStts, buffer);//, true);
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- SEND DIG/AN SENSORS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendDigAnStates()
{
  return;
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  //root[json_homed] = clockHomed ? mqtt_cmdOn : mqtt_cmdOff;

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);
  
  mqttClient.publish(mqtt_controlStts, buffer);
  mqttClient.loop();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = mqtt_cmdOn;
  root[json_version] = version;
  root[json_date] = date;

#ifdef INTERNAL_EN
  dtostrf(DHTTempIn, 1, 1, result);
  root[json_tempin] = result;

  dtostrf(DHTHumIn, 1, 1, result);
  root[json_humin] = result;
#endif
#ifdef EXTERNAL_EN
  dtostrf(DHTTempOut, 1, 1, result);
  root[json_tempout] = result;

  dtostrf(DHTHumOut, 1, 1, result);
  root[json_humout] = result;
#endif

  mqttClient.loop();
  
  /*dtostrf(voltage12V, 1, 1, result);
  root[json_12v] = result;*/

  dtostrf(voltage5V, 1, 1, result);
  root[json_5v] = result;
  
  #if defined(ESP32) || defined(ESP8266)
    root[json_ssid] = WiFi.SSID();
    
    dtostrf(rssi, 1, 2, result);
    root[json_rssi] = result;
  
    dtostrf(rssiPercent, 1, 2, result);
    root[json_rssiPercent] = result;
  #endif

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_sensorJson, buffer);
  mqttClient.loop();
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);
  mqttClient.loop();
}
