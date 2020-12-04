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

    delay(10000);
  }

  if (root.containsKey(json_resetAll))
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(root[json_resetAll], mqtt_cmdOn) != 0)
      return;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);

    pinMode(selfResetPin, OUTPUT);
    digitalWrite(selfResetPin, LOW);
    digitalWrite(selfResetPin, HIGH);

    delay(10000);
  }

  //Change PWM values offset for switch
  if (root.containsKey(json_ledPWM))
  {
    uint8_t levelIn = root[json_ledPWM];
    if (levelIn>20 && levelIn<=100)
      PWM_OFFSET = levelIn;
  }

  //Enable onboard LED for dimmer chip
  if (root.containsKey(json_ledRem))
  {
    if (strcmp(root[json_ledRem], mqtt_cmdOn) == 0)
      enableRemoteLED = true;
    else if (strcmp(root[json_ledRem], mqtt_cmdOff) == 0)
      enableRemoteLED = false;
  }

  //Enable onboard local LED
  if (root.containsKey(json_ledEsp))
  {
    if (strcmp(root[json_ledEsp], mqtt_cmdOn) == 0)
      enableBoardLED = true;
    else if (strcmp(root[json_ledEsp], mqtt_cmdOff) == 0)
      enableBoardLED = false;
  }
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS LED JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
bool processLightJson(char* message)
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
      lampMQTT.state = true;
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
      lampMQTT.state = false;

    if (lampMQTT.state)
      lampMQTT.level = constrain(lampMQTT.level, lampMin, lampMax);

    lampMQTT.full = false;
  }

  if (root.containsKey(json_brightness))
  {
    uint8_t levelIn = root[json_brightness];
    if (levelIn>0)
      lampMQTT.level = levelIn;
    lampMQTT.state = levelIn>0;
    lampMQTT.full = false;
  }

  if (root.containsKey(json_fade))
  {
    if (strcmp(root[json_fade], mqtt_cmdOn) == 0)
      lampMQTT.fade = true;
    else if (strcmp(root[json_fade], mqtt_cmdOff) == 0)
      lampMQTT.fade = false;
  }
  
  return true;
}

bool processFanJson(char* message)
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
      fanMQTT.state = true;
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
      fanMQTT.state = false;

    if (fanMQTT.state)
      fanMQTT.level = constrain(fanMQTT.level, fanMin, fanMax);

    fanMQTT.full = false;
  }

  if (root.containsKey(json_brightness))
  {
    uint8_t levelIn = root[json_brightness];
    if (levelIn>0)
      fanMQTT.level = levelIn;
    fanMQTT.state = levelIn>0;
    fanMQTT.full = false;
  }

  if (root.containsKey(json_fade))
  {
    if (strcmp(root[json_fade], mqtt_cmdOn) == 0)
      fanMQTT.fade = true;
    else if (strcmp(root[json_fade], mqtt_cmdOff) == 0)
      fanMQTT.fade = false;
  }
  
  return true;
}

void sendLightState()
{
  if (!networkActive)
    return;
 
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = lampMQTT.state ? mqtt_cmdOn : mqtt_cmdOff;

  root[json_brightness] = lampMQTT.level;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_lmpStts, buffer);
}

void sendFanState()
{
  if (!networkActive)
    return;
    
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root[json_state] = fanMQTT.state ? mqtt_cmdOn : mqtt_cmdOff;

  root[json_brightness] = fanMQTT.level;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_fanStts, buffer);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  if (!networkActive)
    return;
    
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = mqtt_cmdOn;

  dtostrf(voltage5VDimmer, 4, 1, result); // Leave room for too large numbers!
  root[json_5vDim] = result;

  dtostrf(voltage5VSwitch, 4, 1, result); // Leave room for too large numbers!
  root[json_5vSw] = result;

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
