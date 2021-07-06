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
    ESP.restart();
  }

  if (doc[json_homed])
  {
    if (strcmp(doc[json_homed], mqtt_cmdOn) == 0)
      clockHomed = true;
    else if (strcmp(doc[json_state], mqtt_cmdOff) == 0)
      clockHomed = false;

    if (!clockHomed)
    {
      initialHourOns = false;
      initialMinOns = false;
      initialSecOns = false;
    }
  }
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- SEND DIG/AN SENSORS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendDigAnStates()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  root[json_homed] = clockHomed ? mqtt_cmdOn : mqtt_cmdOff;

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

  dtostrf(voltage5V, 1, 1, result);
  if (voltage5V>3.8)
    root[json_5v] = result;
  else
    root[json_3v3] = result;

//  dtostrf(voltage12V, 1, 1, result); // Leave room for too large numbers!
//  root[json_12v] = result;

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
