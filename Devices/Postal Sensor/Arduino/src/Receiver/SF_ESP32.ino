// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WIFI SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWifi()
{
  Sprint("Init WiFi...");
  //Start WiFi communication
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gw, sn);           // Disable this line to run on DHCP
  WiFi.begin(ssid, password);

  long lastReading = millis();
 
  //Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    flashBoardLed(100, 1);
    if (millis()-lastReading <= 10000)  //Maximum 10s WiFi retry
      wdtReset();
  }

  Sprintln("Connected!");

  networkActive = true;
  
  lastMillis = millis()-10000;
}

bool checkNetwork()
{
  bool networkActive = (WiFi.status() == WL_CONNECTED);
  if (!networkActive)
    initWifi();

  return networkActive;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- OTA SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initOTA()
{
  Sprintln("Init OTA...");
  
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(mqtt_deviceName);
  ArduinoOTA.setPassword(password);

  ArduinoOTA.onStart([]()
  {
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
}

void runOTA()
{
  ArduinoOTA.handle();
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void interruptReboot()
{
  writeEEPROM(); //Save latest data before reboot
  Sprintln("Rebooting...");
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
