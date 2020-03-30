// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- MQTT JSON CONTROLS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void receiveJSON(char* message)
{
  if (!processJson(message) || initDisplays)
    return;
    
  sendState();
}

bool processJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln("parseObject() failed");
    return false;
  }

  if (root.containsKey("state"))
  {
    if (strcmp(root["state"], mqtt_cmdOn) == 0)
      stateOn = true;
    else if (strcmp(root["state"], mqtt_cmdOff) == 0)
      stateOn = false;

    Sprint("State: ");
    Sprintln(stateOn ? mqtt_cmdOn : mqtt_cmdOff);
  }

  if (root.containsKey("transition"))
  {
    timeTextScrollSpeed = map(root["transition"], 0, 100, 1000, 0);
    tempTextScrollSpeed = map(root["transition"], 0, 100, 1000, 0);

    Sprint("Transition: ");
    Sprintln(timeTextScrollSpeed);
  }

  if (root.containsKey("effect") && !initDisplays)
  {
    colorAutoSwitch = false;
    colorfade = true;
    changeColor = false;
    
    if (strcmp(root["effect"], "RainbowCycle") == 0)
      displayFeature = 1;
    else if (strcmp(root["effect"], "TextColor") == 0)
      displayFeature = 2;
    else if (strcmp(root["effect"], "Spoon") == 0)
      displayFeature = 3;
    else if (strcmp(root["effect"], "VertRainbowCycle") == 0)
      displayFeature = 4;
    else if (strcmp(root["effect"], "TextChaser") == 0)
      displayFeature = 5;
    else if (strcmp(root["effect"], "HorizRainbowCycle") == 0)
      displayFeature = 6;
    else
    {
      displayFeature = 0;
      colorfade = false;
    }

    if (strcmp(root["effect"], "AllEffectsRolling") == 0)
    {
      colorAutoSwitch = true;
      displayFeature = 1;
      rainbowIndex = 0;
      initDisplays = false;
    }

    Sprint("Display feature: ");
    Sprintln(displayFeature);
  }
  
  if (root.containsKey("brightness"))
  {
    brightness = root["brightness"];
    Sprint("Brightness: ");
    Sprintln(brightness);
  }
  
  if (root.containsKey("color"))
  {
    colorAutoSwitch = false;
    colorfade = false;
    changeColor = true;
    displayFeature = 0;
    
    red = root["color"]["r"];
    green = root["color"]["g"];
    blue = root["color"]["b"];

    Sprint("RGB: ");
    Sprint(red);
    Sprint(", ");
    Sprint(green);
    Sprint(", ");
    Sprintln(blue);
  }

  return true;
}

void sendState()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  root["state"] = (stateOn) ? mqtt_cmdOn : mqtt_cmdOff;
  
  JsonObject& color = root.createNestedObject("color");
  color["r"] = red;
  color["g"] = green;
  color["b"] = blue;

  root["brightness"] = brightness;
  root["transition"] = map(timeTextScrollSpeed, 1000, 0, 0, 100);
  
  if (colorfade)
  {
    if (colorAutoSwitch)
      root["effect"] = "AllEffectsRolling";
    else if (displayFeature == 1)
      root["effect"] = "RainbowCycle";
    else if (displayFeature == 2)
      root["effect"] = "TextColor";
    else if (displayFeature == 3)
      root["effect"] = "Spoon";
    else if (displayFeature == 4)
      root["effect"] = "VertRainbowCycle";
    else if (displayFeature == 5)
      root["effect"] = "TextChaser";
    else if (displayFeature == 6)
      root["effect"] = "HorizRainbowCycle";
  }
  else
    root["effect"] = "null"; //"Default"

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_statusTopic, buffer, true);
}
