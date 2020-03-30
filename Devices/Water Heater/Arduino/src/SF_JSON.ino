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

  if (root.containsKey(json_state))
  {
    waterHeaterOn = strcmp(root[json_state], mqtt_cmdOn) == 0;
    writeEEPROM();
    
    updatePublish = true;
  }
}

void sendWaterHeaterStates()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = waterHeaterOn ? mqtt_cmdOn : mqtt_cmdOff;

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

  dtostrf(inputWaterTemp, 6, 1, result); // Leave room for too large numbers!
  root[json_tempin] = result;

  dtostrf(outputWaterTemp, 6, 1, result); // Leave room for too large numbers!
  root[json_tempout] = result;

  dtostrf(inputWaterFlow, 6, 1, result); // Leave room for too large numbers!
  root[json_flow] = result;
  
  dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
  root[json_rssi] = result;

  dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
  root[json_rssiPercent] = result;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_sensorJson, buffer);
}
