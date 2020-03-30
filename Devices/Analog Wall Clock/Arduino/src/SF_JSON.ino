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

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
      clockHomed = true;
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
      clockHomed = false;

    Sprint("Homed: ");
    Sprintln(clockHomed ? mqtt_cmdOn : mqtt_cmdOff);

    if (!clockHomed)
    {
      initialHourOns = false;
      initialMinOns = false;
      initialSecOns = false;
    }
  }

  if (root.containsKey(json_reset))
  {
    mqttClient.publish(mqtt_clockStts, "Reset Requested...", false);
    
    if (strcmp(root[json_reset], mqtt_cmdOn) != 0)
      return;

    mqttClient.publish(mqtt_clockStts, "Rebooting...", false);
      
    Sprint("Reset Requested...");
    local_delay(500);
    Sprintln(" rebooting...");
    ESP.restart();
  }
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
  String str = "";

  root[json_state] = clockHomed ? mqtt_cmdOn : mqtt_cmdOff;

  str = String(rssi);
  str.toCharArray(result,chrLngt);
  root[json_rssi] = result;

  str = String(rssiPercent);
  str.toCharArray(result,chrLngt);
  root[json_rssiPercent] = result;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  Sprintln(buffer);

  mqttClient.publish(mqtt_clockStts, buffer, false);
}
