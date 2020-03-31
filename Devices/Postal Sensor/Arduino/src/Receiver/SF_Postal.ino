// ----------------------------------------------------------------------------------------------------
// ------------------------------------- INIT POSTAL SENSOR COLOR -------------------------------------
// ----------------------------------------------------------------------------------------------------
void initPostal()
{
  Sprintln("Init Postal Alarm...");
  initLora();
  
  initEEPROM();

  newPostalStatus = true;
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- PROCESS POSTAL JSON ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void processPostalJson(char* message)
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

  //Postal Test
  //ALWAYS END WITH A DELIMITER ==> {"test": "ID:1 or 0:Vcc:"}
  //Send like this ==> {test: "327:1:3.27:"}
  if (root.containsKey(json_test))
  {
    String test_data = root[json_test].as<String>();

    Sprint("LoRa data to parse: ");
    Sprintln(test_data);
    
    Rssi = "-25";
    newStatusForced = false;
    parseData(test_data);
    
    oledMillis = millis() + OledDelayOff;

    newPostalStatus = true;

    flashBoardLed(100, 4);

    return;
  }

  if (root.containsKey(json_state))
  {
    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
      mailPresent = true;
    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
      mailPresent = false;

    delayMailPresent = 0; //Clear delay timer
    newStatusForced = true;
  }

  if (root.containsKey(json_delay))
  {
    if (strcmp(root[json_delay], mqtt_cmdOn) == 0)
      enableFiveMinuteDelay = true;
    else if (strcmp(root[json_delay], mqtt_cmdOff) == 0)
      enableFiveMinuteDelay = false;
  }

  writeEEPROM();
  flashBoardLed(100, 3);
}

void sendPostalState()
{
  Sprintln("Send Postal State...");
  
  newPostalStatus = false;
  
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 5;  // Buffer big enough for 4-character float
  char charVcc[chrLngt];
  char charBat[chrLngt];
  char charId[chrLngt];
  char charRssi[chrLngt];
  char charWiFi[chrLngt];

  Vcc.toCharArray(charVcc, chrLngt);
  Bat.toCharArray(charBat, chrLngt);
  Id.toCharArray(charId, chrLngt);
  Rssi.toCharArray(charRssi, chrLngt);

  root[json_state] = mailPresent ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_postalVcc] = charVcc;
  root[json_postalBat] = charBat;
  root[json_postalId] = charId;
  root[json_postalRssi] = charRssi;
  root[json_postalSource] = newStatusForced ? json_Forced : json_Sender;
  root[json_delay] = enableFiveMinuteDelay ? mqtt_cmdOn : mqtt_cmdOff;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_postalStts, buffer, true);    //Exceptionnaly, retain message for restart purposes
}
