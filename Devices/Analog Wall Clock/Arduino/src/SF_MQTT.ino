// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- MQTT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initMQTT()
{
  Sprintln("Init MQTT...");
  mqttClient.setServer(mqtt_server, 1883);
  mqttClient.setCallback(mqttCallback);

  mqttClient.setKeepAlive(keepAlive);
  mqttClient.setBufferSize(packetSize);
  mqttClient.setSocketTimeout(socketTimeout);
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

    randomSeed(analogRead(0));
    
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
      //Reset board ONLY if clock not homed or time has been lost
      if (clockHomed && localTimeValid)
        count = 0;
    }
  }
}

void mqttReceive(char* topic, char* message)
{
  if (strcmp(topic,mqtt_timeCmd)==0)
  {
    parseTime(message);
    
    //flashBoardLed(100, 3);
  }
  
  if (strcmp(topic,mqtt_controlCmd)==0)
  {
    //flashBoardLed(100, 2);

    processCommandJson(message);
  }

  updatePublish = true;
}

//Functions to run while trying to reconnect
void mqttKeepRunning()
{
  wdtReset();

  runOTA();

  readSensors(false);

  initClock();
  
  //Keep updating clock even if MQTT is down
  if (clockHomed)
  {
    updateTime();
    updateClock();
  }

  writeOutputs();

  runEvery5seconds();
  runEvery10seconds();
}

void mqttSubscribe()
{
  mqttClient.subscribe(mqtt_controlCmd);
  mqttClient.subscribe(mqtt_timeCmd);
  mqttClient.loop();
}

void mqttPublish()
{
  //Update on request when network available
  if ((!updatePublish and localTimeValid) || !networkActive)
    return;

  if (!localTimeValid && (lastTimeRequestMillis==0 || millis()-lastTimeRequestMillis>5000))
  {
    Sprintln("Time Request...");
    mqttClient.publish(mqtt_timeRequest, mqtt_cmdOn);

    lastTimeRequestMillis = millis();
  }
    
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
