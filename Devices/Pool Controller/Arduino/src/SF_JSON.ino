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

    if (ENABLE_WDT)
      forceReset();
  }

  if (root.containsKey(json_drain))
  {
    int rawValue = root[json_drain];
    if (rawValue >= 0 && rawValue <= 100)
      flowSkimDrain = rawValue;

    Sprintln(flowSkimDrain);
  }

  if (root.containsKey(json_slide))
  {
    int rawValue = root[json_slide];
    if (rawValue >= 0 && rawValue <= 100)
      flowSlide = rawValue;

    Sprintln(flowSlide);
  }

  if (root.containsKey(json_heatp))
  {
    int rawValue = root[json_heatp];
    if (rawValue >= 0 && rawValue <= 100)
      flowHeater = rawValue;

    Sprintln(flowHeater);
  }
}

void sendCommandState()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_drain] = flowSkimDrain;
  root[json_slide] = flowSlide;
  root[json_heatp] = flowHeater;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_controlStts, buffer);//, true);
}


// ----------------------------------------------------------------------------------------------------
// --------------------------------------- PROCESS JSON RELAYS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void processRelayJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return false;
  }

  for (int i=0; i<relayQty; i++)
  {
    if (root.containsKey(jsonRelay[i]))
      relayCmd[i] = strcmp(root[jsonRelay[i]], mqtt_cmdOn) == 0;
  }
}

void sendRelayStates()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  for (int i=0; i<relayQty; i++)
    root[jsonRelay[i]] = relayCmd[i] ? mqtt_cmdOn : mqtt_cmdOff;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_relayStts, buffer);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- PROCESS JSON DOOR/LOCK ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void processDoorJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (root.containsKey(json_lock))
    doorLockRequest(strcmp(root[json_lock],json_lock)==0);
}

void sendDoorStates()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_closed] = poolCabinDoor ? json_closed : json_opened;
  root[json_locked] = poolCabinLocked ? json_locked : json_unlocked;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_doorStts, buffer);
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

  root[json_wtemp] = poolTempIn;
  root[json_htemp] = poolTempOut;
  root[json_ptemp] = poolPumpTemp;
  
  root[json_wpsi] = poolPress;
  root[json_wph] = poolPH;

  root[json_hilvl] = poolLvlHi ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_lolvl] = poolLvlLo ? mqtt_cmdOn : mqtt_cmdOff;
  
  dtostrf(DHTTempIn, 4, 1, result); // Leave room for too large numbers!
  root[json_tempin] = result;

  dtostrf(DHTHumIn, 5, 1, result); // Leave room for too large numbers!
  root[json_humin] = result;

  dtostrf(DHTTempOut, 4, 1, result); // Leave room for too large numbers!
  root[json_tempout] = result;

  dtostrf(DHTHumOut, 5, 1, result); // Leave room for too large numbers!
  root[json_humout] = result;

  dtostrf(voltage5V, 4, 1, result); // Leave room for too large numbers!
  root[json_5v] = result;

  dtostrf(voltage12V, 5, 1, result); // Leave room for too large numbers!
  root[json_12v] = result;

  #ifdef ESP32 || ESP8266
    dtostrf(rssi, 6, 2, result); // Leave room for too large numbers!
    root[json_rssi] = result;
  
    dtostrf(rssiPercent, 6, 2, result); // Leave room for too large numbers!
    root[json_rssiPercent] = result;
  #endif

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
}
