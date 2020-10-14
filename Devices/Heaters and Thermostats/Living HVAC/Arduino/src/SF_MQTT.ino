// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initMQTT()
{
  Sprintln("Init MQTT...");
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttCallback);
}

bool runMQTT()
{
  bool statusMqtt = mqttClient.connected();
  if (!statusMqtt)
    reconnect();

  mqttClient.loop();

  return statusMqtt;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- MQTT Callback --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  wdtReset(); //Added to prevent reboot when a bunch of data gets in all at once
  
  Sprint("Message arrived [");
  Sprint(topic);
  Sprint("] ");

  if (length >= BUFFER_ARRAY_SIZE)
    length = BUFFER_ARRAY_SIZE-1;

  for (int i = 0; i < length; i++)
    message[i] = (char)payload[i];
    
  message[length] = '\0';
  Sprintln(message);

  wdtReset(); //Added to prevent reboot when a bunch of data gets in all at once
  local_delay(5);

  mqttReceive(topic, message);
}

void reconnect()
{
  // Loop until we're reconnected
  uint8_t count = 0;
  
  while (!mqttClient.connected())
  {
    Sprint("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = mqtt_deviceName;
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_password, mqtt_willTopic, mqtt_willQoS, mqtt_willRetain, mqtt_willOffline))
    {
      Sprintln("connected");
      // Once connected, publish an announcement...
      updatePublish = true;
      mqttPublish();
      // ... and resubscribe
      mqttSubscribe();
    }
    else
    {
      Sprint("failed, rc=");
      Sprint(mqttClient.state());
      Sprintln(" try again in 5 seconds");

      long lastReading = millis();

      // Wait 5 seconds before retrying
      while (millis()-lastReading < 5000)
      {
        //Keep reading data even if MQTT is down
        if (count < 5) //Max 5 tries
          mqttKeepRunning();
                 
        local_delay(1000);
      }
      count++;
    }
  }
}

void mqttReceive(char* topic, char* message)
{
  if (strcmp(topic,mqtt_controlCmd)==0)
  {
    //flashBoardLed(100, 2);

    processCommandJson(message);
  }

  bool dataFromThermostat = false;

  //HVAC settings
  //Integrated POWER ON/OFF into other commands
  /*if (strcmp(topic,mqtt_powerCmd)==0)
  {
    setStateMQTT(message);

    flashBoardLed(100, 1);
  }
  else*/ if (strcmp(topic,mqtt_modeCmd)==0)
  {
    setModeMQTT(message);

    //flashBoardLed(100, 2);
  }
  else if (strcmp(topic,mqtt_tempCmd)==0)
  {
    setTempMQTT(message);

    //flashBoardLed(100, 3);
  }
  else if (strcmp(topic,mqtt_fanCmd)==0)
  {
    setFanMQTT(message);

    //flashBoardLed(100, 4);
  }
  else if (strcmp(topic,mqtt_swingCmd)==0)
  {
    setSwingMQTT(message);

    //flashBoardLed(100, 5);
  }
  else if (strcmp(topic,mqtt_thermJson)==0)
  {
    processThermJson(message);

    //flashBoardLed(100, 6);

    dataFromThermostat = true;
  }
  else
  {
    sendDataToHvac = false;
    updatePublish = true;
    return;
  }

  if (!dataFromThermostat)
  {
    sendDataToHvac = true;
    updatePublish = true;
    runEvery10seconds(true);
  }
}

//Functions to run while trying to reconnect
void mqttKeepRunning()
{
  wdtReset();

  runOTA();
  
  //readSensors();          //Read sensors (buttons, etc.)

  updateTime();

  writeOutputs();           //Set OUTPUTS devices

  runEvery5seconds();
  runEvery10seconds(false);
}

void mqttSubscribe()
{
  mqttClient.subscribe(mqtt_powerCmd);
  mqttClient.subscribe(mqtt_modeCmd);
  mqttClient.subscribe(mqtt_tempCmd);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_fanCmd);
  mqttClient.subscribe(mqtt_swingCmd);
  mqttClient.subscribe(mqtt_thermJson);
  mqttClient.loop();
}

void mqttPublish()
{
  //Update on request when network available
  if (!updatePublish || !networkActive)
    return;

  //TODO
    
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);

  updatePublish = false;
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- LOCAL YIELD/DELAY -----------------------------------------
// ----------------------------------------------------------------------------------------------------
// Need a local yield/delay that that calls yield() and mqttClient.loop()
//   The system yield()/delay() routine does not call mqttClient.loop()

void local_yield()
{
  yield();
  mqttClient.loop();
}

void local_delay(unsigned long millisecs)
{
  unsigned long start = millis();
  local_yield();
  if (millisecs > 0)
  {
    while (millis()-start < millisecs)
      local_yield();
  }
}