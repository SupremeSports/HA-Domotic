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
                 
        local_delay(50);
      }
      count++;
    }
  }
}

void mqttReceive(char* topic, String data, char* message)
{
  if (strcmp(topic,mqtt_timeValue)==0)
    parseTime(data);
  else if (strcmp(topic,mqtt_waterTemp)==0)
    getWaterTemp(data);
  else if (strcmp(topic,mqtt_waterPH)==0)
    getWaterPH(data);
  else if (strcmp(topic,mqtt_outTemp)==0)
    getOutTemp(data);
  else if (strcmp(topic,mqtt_outHum)==0)
    getOutHum(data);
  else if (strcmp(topic,mqtt_text_time)==0)
    timeTextStringBuffer = padString(data, timeDigits);
  else if (strcmp(topic,mqtt_text_temp)==0)
    tempTextStringBuffer = padString(data, tempDigits);
  else
    receiveJSON(message);

  updatePublish = true;
}

//Functions to run while trying to reconnect
void mqttKeepRunning()
{
  //Keep updating data even if MQTT is down
  if (localTimeValid)
  {
    readSensors();            //Read sensors (buttons, etc.)

    updateTime();             //Display current time
    updateTemp();             //Display outdoor temperature/humidity
    updateWater();            //Display water temperature
  
    updateTimeText();         //Scroll text display (if in scroll mode)
    updateTempText();         //Scroll text display (if in scroll mode)
  
    autoColorChanging();      //Control colors and string output

    //Short flash every 5 seconds when everything is ok
    if (prevSecond != Second && (Second%5) == 0)
      flashBoardLed(2, 1);

    if (second() != Second)
      updatePublish = true;
  
    prevSecond = Second;
    prevMinute = Minute;
    //prevHour = Hour;        //Not needed
  }  
}

void mqttSubscribe()
{
  mqttClient.subscribe(mqtt_timeValue);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_waterTemp);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_waterPH);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_outTemp);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_outHum);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_text_time);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_text_temp);
  mqttClient.loop();
  mqttClient.subscribe(mqtt_cmdTopic);
  mqttClient.loop();

  sendState();
}

void mqttPublish()
{
  //Update on request when network available
  if (!updatePublish || !networkActive)
    return;

  mqttClient.publish(mqtt_willTopic, mqtt_willOnline);

  mqttClient.publish(mqtt_timeRequest, localTimeValid ? mqtt_cmdOff : mqtt_cmdOn);

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
