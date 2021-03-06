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
  if (strcmp(topic,mqtt_timeCmd)==0)
  {
    flashBoardLed(100, 1);
    
    parseTime(data);
  }
  
  if (strcmp(topic,mqtt_clockCmd)==0)
  {
    flashBoardLed(100, 2);

    processCommandJson(message);
  }

  updatePublish = true;
}

//Functions to run while trying to reconnect
void mqttKeepRunning()
{
  wdtReset();

  runOTA();

  readSensors();

  initClock();
  
  //Keep updating clock even if MQTT is down
  if (clockHomed)
  {
    updateTime();
    updateClock();
  }

  writeOutputs();

  flashEvery5sec();
}

void mqttSubscribe()
{
  mqttClient.subscribe(mqtt_clockCmd);
  mqttClient.subscribe(mqtt_timeCmd);
  mqttClient.loop();
}

void mqttPublish()
{
  //Update on request when network available
  if (!updatePublish || !networkActive)
    return;

  bool retain = true;

  if (!localTimeValid)
    mqttClient.publish(mqtt_timeRequest, mqtt_cmdOn);

  if (!clockHomed)
    sendSensors();
    
  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);

  mqttClient.loop();

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
