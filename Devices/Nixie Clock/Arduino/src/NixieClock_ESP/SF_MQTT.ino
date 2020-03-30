// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initMQTT()
{
  Sprintln("Init MQTT...");
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttCallback);

  local_delay(10);
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);
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
  String data = "";
  Sprint("Message arrived [");
  Sprint(topic);
  Sprint("] ");

  char message[length + 1];
  for (int i = 0; i < length; i++)
  {
    Sprint((char)payload[i]);
    data += (char)payload[i];
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  Sprintln();

  mqttReceive(topic, data, message);
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
      Sprintln("Connected!");
      // Once connected, publish an announcement...
      updatePublish = true;
      mqttPublish();
      prevMinute = 100;    //Force data update
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
                 
        local_delay(50);
      }
      count++;
    }
  }
}

void mqttReceive(char* topic, String data, char* message)
{
  //Time update received
  if (strcmp(topic,mqtt_timeCmd)==0)
  {
    parseTime(data);
    
    flashBoardLed(100, 3);

    updatePublish = true;    //Force data update
  }
    
  //Nixie Clock settings
  if (strcmp(topic,mqtt_clockCmd)==0)
  {
    bool dataOk = processCommandJson(message);

    //flashBoardLed(100, 2);

    if (dataOk)
      prevMinute = 100; //Force data update
  }

  //Backlights RGB settings
  if (strcmp(topic,mqtt_ledCmd)==0)
  {
    bool dataOk = processLightColorsJson(message);

    //flashBoardLed(100, 1);

    if (dataOk)
      prevMinute = 100; //Force data update
  }
}

//Functions to run while trying to reconnect
void mqttKeepRunning()
{
  wdtReset();

  runOTA();

  readSensors();

  updateTime();

  writeOutputs();

  flashEvery5sec();
}

void mqttSubscribe()
{
  mqttClient.subscribe(mqtt_clockCmd);
  mqttClient.subscribe(mqtt_ledCmd);
  mqttClient.subscribe(mqtt_timeCmd);
  mqttClient.loop();
}

void mqttPublish()
{
  //Update on request when network available
  if ((!updatePublish and localTimeValid) || !networkActive)
    return;

  if (!localTimeValid)
    mqttClient.publish(mqtt_timeRequest, mqtt_cmdOn);
    
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);

  //mqttClient.loop();

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
