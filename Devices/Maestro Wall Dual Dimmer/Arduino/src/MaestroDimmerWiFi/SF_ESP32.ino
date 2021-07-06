// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WIFI SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWifi()
{
  if (testStandalone)
    standaloneMode = true;
    
  if (!standaloneMode)
    Sprintln("Init WiFi...");
    
  //Start WiFi communication
  WiFi.mode(WIFI_STA);
  //WiFi.config(ip, gw, sn);           // Disable this line to run on DHCP
  //WiFi.config(ip, dns, gw, sn);           // Disable this line to run on DHCP
  WiFi.config(ip,  gw, sn, dns1, dns2);           // Disable this line to run on DHCP

  long reconnectDelay = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    wdtReset();
    networkActive = false;
    WiFi.disconnect();

    Sprint("SSID: ");
    Sprint(ssid);
    Sprint(" - ");
    
    standaloneMode = !digitalRead(pinStandalone) && !testStandalone;
    if (standaloneMode || testStandalone)
    {
      WiFi.begin(ssid, passwordWrong);
      Sprintln(passwordWrong);
    }
    else
    {
      WiFi.begin(ssid, password);
      Sprintln(password);
    }

    while (WiFi.status() != WL_CONNECTED && (millis()-reconnectDelay<3000))
      runStandalone();

    if (WiFi.status() == WL_CONNECTED)
      break; 

    reconnectDelay = millis();
  }

  if (!standaloneMode)
    Sprintln("WiFi Connected!");

  networkActive = checkNetwork();
  
  lastSecond = millis()-10000;
}

bool checkNetwork()
{
  standaloneMode = !digitalRead(pinStandalone) && !testStandalone;
  bool networkActive = (WiFi.status() == WL_CONNECTED);
 
  if (!networkActive || standaloneMode)
    initWifi();

  getQuality();

  return networkActive;
}

/*
   Return the quality (Received Signal Strength Indicator)
   of the WiFi network.
   Returns a number between 0 and 100 if WiFi is connected.
   Returns -1 if WiFi is disconnected.
*/
void getQuality()
{
  if (!networkActive)
  {
    rssi = -120;
    rssiPercent -1;
    return;
  }
  int dBm = WiFi.RSSI();
  if (dBm <= -100)
    rssiPercent = 0;
  else if (dBm >= -50)
    rssiPercent = 100;
  else
    rssiPercent = 2 * (dBm + 100);
  
  rssi = dBm;
}

//Standalone running without wifi successful
void runStandalone()
{
  mqttKeepRunning(); 
  
  wdtReset();
  
  newStart = false;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- OTA SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initOTA()
{
  #ifdef ENABLE_OTA
    Sprintln("Init OTA...");

    #ifdef ESP32
      ArduinoOTA.setPort(3232);
    #elif ESP8266
      ArduinoOTA.setPort(8266);
    #endif

    ArduinoOTA.setHostname(mqtt_deviceName);
    ArduinoOTA.setPassword(password);
  
    ArduinoOTA.onStart([]()
    {
      enableBoardLED = true;
      mqttClient.disconnect(); // Disconnect MQTT
      
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
  
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Sprintln("Start updating " + type);
    });
    ArduinoOTA.onEnd([]()
    {
      Sprintln("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) 
    {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      wdtReset();  //Keep feeding the dog while uploading data, otherwise it will reboot
      flashBoardLed(1,1); //Flash led during upload (slows down a little bit, but at least you know it works)
    });
    ArduinoOTA.onError([](ota_error_t error)
    {
     Serial.printf("Error[%u]: ", error);
     if (error == OTA_AUTH_ERROR) Sprintln("Auth Failed");
     else if (error == OTA_BEGIN_ERROR) Sprintln("Begin Failed");
     else if (error == OTA_CONNECT_ERROR) Sprintln("Connect Failed");
     else if (error == OTA_RECEIVE_ERROR) Sprintln("Receive Failed");
     else if (error == OTA_END_ERROR) Sprintln("End Failed");
    });
  
    ArduinoOTA.begin();
  #else
    Sprintln("OTA Disabled...");
    #error YOU SHOULD NOT UPLOAD WITHOUT OTA
  #endif
}

void runOTA()
{
  #ifdef ENABLE_OTA
    ArduinoOTA.handle();
  #endif
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void interruptReboot()
{
  Sprintln(json_resetReboot);
  ESP.restart();
}

void initWDT()
{
  Sprintln("Init Watchdog...");
  watchdogTimer = timerBegin(0, 80, true);                      //Timer0, divisor 80, countup
  timerAlarmWrite(watchdogTimer, LWD_TIMEOUT*1000, false);      //Set time in uS
  timerAttachInterrupt(watchdogTimer, & interruptReboot, true);
  timerAlarmEnable(watchdogTimer);                              //Enable interrupt
}

void wdtReset()
{
  //Reset timer feed dog
  timerWrite(watchdogTimer, 0);
}
