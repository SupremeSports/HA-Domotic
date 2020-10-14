// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS HVAC JSON ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void processCommandJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (root.containsKey(json_reset))
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(root[json_reset], mqtt_cmdOn) != 0)
      return;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);
    ESP.restart();
  }
}

void processThermJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
    {
      lastKeepAlive = 0; //Reset counter
      thermAlive = true;
    }
  }

  if (root.containsKey(json_roomTemp))
    roomAirTemp = atof(root[json_roomTemp]);
  if (root.containsKey(json_roomHum))
    roomAirHum = atof(root[json_roomHum]);
}

void sendHvacStates()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = configStateOn ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_temp] = configTempSetpoint;
  root[json_mode] = runModes[configRunMode];
  root[json_fan] = fanModes[configFanMode];
  root[json_swing] = configSwingMode ? "On" : "Off";

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

  root[json_therm] = thermAlive ? mqtt_cmdOn : mqtt_cmdOff;

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  dtostrf(outputAirTemp, 6, 1, result); // Leave room for too large numbers!
  root[json_airTemp] = result;

  if (thermAlive && roomAirTemp != initValue && roomAirHum != initValue)
  {
    dtostrf(roomAirTemp, 6, 1, result); // Leave room for too large numbers!
    root[json_roomTemp] = result;

    dtostrf(roomAirHum, 6, 1, result); // Leave room for too large numbers!
    root[json_roomHum] = result;
  }
  else
  {
    root[json_roomTemp] = "---";
    root[json_roomHum] = "---";
  }

  #if defined(ESP32) || defined(ESP8266)
    root[json_ssid] = WiFi.SSID();
    
    dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
    root[json_rssi] = result;
  
    dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
    root[json_rssiPercent] = result;
  #endif

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
}