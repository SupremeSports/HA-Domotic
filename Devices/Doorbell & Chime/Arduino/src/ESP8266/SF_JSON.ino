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

  if (root.containsKey(json_resetnano))
  {
    if (strcmp(root[json_resetnano], mqtt_cmdOn) != 0)
      return;

    mqttClient.publish(mqtt_controlStts, json_resetReq);

    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, LOW);
    local_delay(1000);
    digitalWrite(resetPin, HIGH);
    pinMode(resetPin, INPUT);

    return;
  }


  if (root.containsKey(json_state))
  {
    ringRequested = strcmp(root[json_state], mqtt_cmdOn) == 0;

    updatePublish = true;
  }
  
  if (root.containsKey(json_sound))
  {
    int value = atoi(root[json_sound]);
    Sprintln(value);

    selectedSound = constrain(value, 0, MAXSOUND);

    updatePublish = true;
  }

  if (root.containsKey(json_volume))
  {
    int value = atoi(root[json_volume]);
    Sprintln(value);
    
    selectedVolume = constrain(value, 0, MAXVOL);

    updatePublish = true;
  }

  if (!updatePublish)
    return;

  writeEEPROM();

  clearI2C();
  outI2C[idByte] = soundID;
  outI2C[soundByte] = selectedSound;
  outI2C[volumeByte] = selectedVolume;
  outI2C[ringByte] = ringRequested ? 1 : 0;;

  sendI2C();
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
  
  dtostrf(selectedSound, 6, 0, result); // Leave room for too large numbers!
  root[json_sound] = result;

  dtostrf(selectedVolume, 6, 0, result); // Leave room for too large numbers!
  root[json_volume] = result;

  dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
  root[json_rssi] = result;

  dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
  root[json_rssiPercent] = result;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);
}
