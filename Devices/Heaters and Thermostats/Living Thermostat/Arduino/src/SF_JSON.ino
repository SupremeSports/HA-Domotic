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
    return ;
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

  if (root.containsKey(json_passcode))
  {
    uint16_t value = atoi(root[json_temp]);
    passcode = constrain(value, 1, 65535);
    writeEEPROM();
  }
}

void processHvacStts(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  bool newConnect = false;

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (root.containsKey(json_state))
  {
    configStateOn = strcmp(root[json_state], mqtt_cmdOn)==0;
    lastKeepAlive = 0; //Reset counter
    newConnect = !hvacAlive;
    hvacAlive = true;
  }

  if (root.containsKey(json_temp))
  {
    int value = atoi(root[json_temp]);
    if (configTempSetpoint != value || newConnect)
    {
      configTempSetpoint = value;
      Sprint("Temp set to: ");
      Sprint(configTempSetpoint);
      Sprintln("C");
  
      updateSetTemp();
    }
  }
  if (root.containsKey(json_mode))
  {
    for (int i=RUNMODES_MIN; i<=RUNMODES_MAX; i++)
    {
      if (strcmp(root[json_mode],runModes[i])==0)
      {
        if (configRunMode != i || newConnect)
        {
          configRunMode = i;
          Sprint("Mode set to: ");
          Sprintln(runModes[configRunMode]);
        }
        break;
      }
    }
  }
  
  if (root.containsKey(json_fan))
  {
    for (int i=FANMODES_MIN; i<=FANMODES_MAX; i++)
    {
      if (strcmp(root[json_fan],fanModes[i])==0)
      {
        if (configFanMode != i || newConnect)
        {
          configFanMode = i;
          Sprint("Fan set to: ");
          Sprintln(fanModes[configFanMode]);
      
          updateFanLevel();
        }
        break;
      }
    }
  }

  /*if (root.containsKey(json_swing))
  {
    if (strcmp(root[json_swing],"On")==0)
    {
      if (!configSwingMode || newConnect)
      {
        configSwingMode = true;
        Sprintln("Swing set to ON (unprogrammed)");
        
        //update_swing_mode();
      }
    }
    else if (strcmp(root[json_swing],"Off")==0)
    {
      if (configSwingMode || newConnect)
      {
        configSwingMode = false;
        Sprintln("Swing set to OFF (unprogrammed)");
        
        //update_swing_mode();
      }
    }
  }*/
}

void processHvacJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return ;
  }
  
  if (root.containsKey(json_state))
    stateHVAC = strcmp(root[json_state], mqtt_cmdOn)==0;
  if (root.containsKey(json_airTemp))
  {
    air_temp = atof(root[json_airTemp]);
    //Sprintln(air_temp);
  }
}

void sendCommandPower()
{
  mqttClient.publish(mqtt_powerCmd, configStateOn ? mqtt_cmdOn : mqtt_cmdOff);
}
void sendCommandMode()
{
  mqttClient.publish(mqtt_modeCmd, runModes[configRunMode]);
}
void sendCommandTemp()
{
  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];
 
  dtostrf(configTempSetpoint, 2, 0, result); // Leave room for too large numbers!
  mqttClient.publish(mqtt_tempCmd, result);
}
void sendCommandFan()
{
  mqttClient.publish(mqtt_fanCmd, fanModes[configFanMode]);
}
void sendCommandSwing()
{
  mqttClient.publish(mqtt_swingCmd, configSwingMode ? "On" : "Off");
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

  root[json_state] = mqtt_cmdOn;

  dtostrf(iRoom_temperature, 6, 1, result); // Leave room for too large numbers!
  root[json_roomTemp] = result;

  dtostrf(iRoom_humidity, 6, 1, result); // Leave room for too large numbers!
  root[json_roomHum] = result;

  dtostrf(voltage5V, 4, 1, result); // Leave room for too large numbers!
  root[json_5v] = result;
  
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
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);
}
