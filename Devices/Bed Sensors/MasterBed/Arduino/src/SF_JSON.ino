// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
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

  if (root.containsKey(json_scale))
  {
    int value = atoi(root[json_scale]);

    Sprintln(value);
   
    loadcellCalib = abs(value);
    writeEEPROM();

    setScale();

    updatePublish = true;
  }

  if (root.containsKey(json_tare))
  {
    if (strcmp(root[json_tare], mqtt_cmdOn) != 0)
      return;
    
    setTare();
    writeEEPROM();

    updatePublish = true;
  }
}

void sendCommandJson()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_tare] = mqtt_cmdOff;

  dtostrf(outputScale, 6, 1, result); // Leave room for too large numbers!
  root[json_scale] = result;

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

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];
  
  dtostrf(loadcellCalib, 6, 1, result); // Leave room for too large numbers!
  root[json_scale] = result;

  dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
  root[json_rssi] = result;

  dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
  root[json_rssiPercent] = result;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
}
