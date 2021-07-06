// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void processCommandJson(char* message)
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (doc[json_reset])
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(doc[json_reset], mqtt_cmdOn) != 0)
      return;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);

    if (ENABLE_WDT)
      forceReset();
  }
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- JSON RELAYS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void processRelayJson(char* message)
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Sprintln(json_parseFailed);
    return;
  }

  String relayID = "";
  String holdID = "";

  for (int i=0; i<relayQty; i++)
  {
    relayID = json_relay + String(i+1);
    holdID = json_hold + String(i+1);
    
    if (doc[relayID])
    {
      relayCmd[i] = strcmp(doc[relayID], mqtt_cmdOn) == 0;
      forceMqttUpdate = true;
    }

    relayHld[i] = doc[holdID] ? atof(doc[holdID]) : 0.0;
  }
}

void sendRelayStates()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  for (int i=0; i<(relayQty); i++)
  {
    String relayID = json_relay + String(i+1);
    root[relayID] = relayCmd[i] ? mqtt_cmdOn : mqtt_cmdOff;
  }

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);
  
  mqttClient.publish(mqtt_relayStts, buffer);
  mqttClient.loop();
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- SEND DIG/AN SENSORS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendDigAnStates()
{
  //TODO
  return;
  
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  //TODO

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);
  
  mqttClient.publish(mqtt_sensorsStts, buffer);
  mqttClient.loop();
}

void sendLockStts()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  root[json_mdoor] = mainDoorStatus ? json_closed : json_opened;
  root[json_ldoors] = leftDoorsStatus ? json_closed : json_opened;
  root[json_rdoors] = rightDoorsStatus ? json_closed : json_opened;

  root[json_locked] = mainDoorLockStatus ? json_locked : json_unlocked;
  
  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);
  
  mqttClient.publish(mqtt_lockStts, buffer);
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
  dtostrf(DHTTempIn, 1, 1, result); // Leave room for too large numbers!
  root[json_tempin] = result;

  dtostrf(DHTHumIn, 1, 1, result); // Leave room for too large numbers!
  root[json_humin] = result;
#endif

#ifdef EXTERNAL_EN
  dtostrf(DHTTempOut, 1, 1, result); // Leave room for too large numbers!
  root[json_tempout] = result;

  dtostrf(DHTHumOut, 1, 1, result); // Leave room for too large numbers!
  root[json_humout] = result;
#endif

  mqttClient.loop();

  dtostrf(voltage5V, 1, 1, result);
  root[json_5v] = result;

  dtostrf(voltage12V, 1, 1, result); // Leave room for too large numbers!
  root[json_12v] = result;

  #if defined(ESP32) || defined(ESP8266)
    root[json_ssid] = WiFi.SSID();
    
    dtostrf(rssi, 1, 2, result); // Leave room for too large numbers!
    root[json_rssi] = result;
  
    dtostrf(rssiPercent, 1, 2, result); // Leave room for too large numbers!
    root[json_rssiPercent] = result;
  #endif

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_sensorJson, buffer);
  mqttClient.loop();
}
