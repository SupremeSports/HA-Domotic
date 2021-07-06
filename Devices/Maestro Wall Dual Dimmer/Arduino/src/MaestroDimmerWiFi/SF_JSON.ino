// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
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

    delay(10000);
  }

  if (doc[json_resetAll])
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(doc[json_resetAll], mqtt_cmdOn) != 0)
      return false;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);

    pinMode(selfResetPin, OUTPUT);
    digitalWrite(selfResetPin, LOW);
    digitalWrite(selfResetPin, HIGH);

    delay(10000);
  }

  bool dataUpdate = false;

  //Change PWM values offset for switch
  if (doc[json_ledPWM])
  {
    uint8_t levelIn = doc[json_ledPWM];
    if (levelIn>=20 && levelIn<=100)
    {
      PWM_OFFSET = levelIn;
      dataUpdate = true;
    }
  }

  //Enable onboard LED for dimmer chip
  if (doc[json_ledRem])
  {
    if (strcmp(doc[json_ledRem], mqtt_cmdOn) == 0)
      enableRemoteLED = true;
    else if (strcmp(doc[json_ledRem], mqtt_cmdOff) == 0)
      enableRemoteLED = false;

    dataUpdate = true;
  }

  //Enable onboard local LED
  if (doc[json_ledEsp])
  {
    if (strcmp(doc[json_ledEsp], mqtt_cmdOn) == 0)
      enableBoardLED = true;
    else if (strcmp(doc[json_ledEsp], mqtt_cmdOff) == 0)
      enableBoardLED = false;

    dataUpdate = true;
  }

  return dataUpdate;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS LED JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processLightJson(char* message)
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
      lampMQTT.state = true;
    else if (strcmp(doc[json_state], mqtt_cmdOff) == 0)
      lampMQTT.state = false;

    if (lampMQTT.state)
      lampMQTT.level = constrain(lampMQTT.level, lampMin, lampMax);

    lampMQTT.full = false;
  }

  if (doc[json_brightness])
  {
    uint8_t levelIn = doc[json_brightness];
    if (levelIn>0)
      lampMQTT.level = levelIn;
    lampMQTT.state = levelIn>0;
    lampMQTT.full = false;
  }

  if (doc[json_fade])
  {
    if (strcmp(doc[json_fade], mqtt_cmdOn) == 0)
      lampMQTT.fade = true;
    else if (strcmp(doc[json_fade], mqtt_cmdOff) == 0)
      lampMQTT.fade = false;
  }
  
  return true;
}

bool processFanJson(char* message)
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
      fanMQTT.state = true;
    else if (strcmp(doc[json_state], mqtt_cmdOff) == 0)
      fanMQTT.state = false;

    if (fanMQTT.state)
      fanMQTT.level = constrain(fanMQTT.level, fanMin, fanMax);

    fanMQTT.full = false;
  }

  if (doc[json_brightness])
  {
    uint8_t levelIn = doc[json_brightness];
    if (levelIn>0)
      fanMQTT.level = levelIn;
    fanMQTT.state = levelIn>0;
    fanMQTT.full = false;
  }

  if (doc[json_fade])
  {
    if (strcmp(doc[json_fade], mqtt_cmdOn) == 0)
      fanMQTT.fade = true;
    else if (strcmp(doc[json_fade], mqtt_cmdOff) == 0)
      fanMQTT.fade = false;
  }
  
  return true;
}

void sendLightState()
{
  if (!networkActive)
    return;
 
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = lampMQTT.state ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_brightness] = lampMQTT.level;

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_lmpStts, buffer);
  mqttClient.loop();
}

void sendFanState()
{
  if (!networkActive)
    return;
    
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = fanMQTT.state ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_brightness] = fanMQTT.level;

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_fanStts, buffer);
  mqttClient.loop();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  if (!networkActive)
    return;
    
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = mqtt_cmdOn;
  root[json_version] = version;
  root[json_date] = date;

  dtostrf(voltage5VDimmer, 1, 1, result); // Leave room for too large numbers!
  root[json_5vDim] = result;

  dtostrf(voltage5VSwitch, 1, 1, result); // Leave room for too large numbers!
  root[json_5vSw] = result;

  dtostrf(DHTTempOut, 1, 1, result); // Leave room for too large numbers!
  root[json_tempout] = result;

  dtostrf(DHTHumOut, 1, 1, result); // Leave room for too large numbers!
  root[json_humout] = result;

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
