// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WIFI SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWifi()
{
  Sprint("Init WiFi...");
  //Start WiFi communication
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gw, sn);           // Disable this line to run on DHCP

  int count = 0;

  for (int i = 0; i<ssid_qty; ++i)
  {
    count = 0;
    WiFi.disconnect();
    WiFi.begin(ssid[i], password[i]);
    Sprint("SSID #");
    Sprint(i);
    Sprint(": ");
    Sprint(ssid[i]);
    Sprint(" - ");
    Sprintln(password[i]);
    
    while (WiFi.status() != WL_CONNECTED && count < 5)
    {
      flashBoardLed(100, 5);
      count++;
      wdtReset();
    }
    
    if (WiFi.status() == WL_CONNECTED)
      break;    
  }

  long lastReading = millis();
 
  //Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    flashBoardLed(100, 1);
    if (millis()-lastReading <= 10000)  //Maximum 10s WiFi retry
      wdtReset();
  }

  Sprintln("Connected!");

  networkActive = checkNetwork();
  
  lastSecond = millis()-10000;
}

bool checkNetwork()
{
  bool networkActive = (WiFi.status() == WL_CONNECTED);
  if (!networkActive)
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

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- OTA SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initOTA()
{
  Sprintln("Init OTA...");

  #ifdef ESP32
    ArduinoOTA.setPort(3232);
  #elif ESP8266
    ArduinoOTA.setPort(8266);
  #endif
  
  ArduinoOTA.setHostname(mqtt_deviceName);
  ArduinoOTA.setPassword(password[ssid_qty-1]);

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
}

void runOTA()
{
  ArduinoOTA.handle();
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- CHECK RESTART REASON ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkResetCause()
{
  Sprintln("Check restart reason...");
  struct  rst_info  *rtc_info = system_get_rst_info();
  Sprint("reset  reason: ");
  Sprintln(rtc_info->reason);

  //Don't light screen up if it's not booting from a power up event
  if (rtc_info->reason != 6)
  {
    screenOff = true;
    ledMillis = millis()-1;

    //PMode = PM_MAIN;
  }
  else
  {
    ledMillis = millis() + ledDelayOff*1000;
    //PMode = PM_BOOT;
  }
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifdef ESP32
  void interruptReboot()
  {
    writeEEPROM(); //Save latest data before reboot
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
#elif ESP8266
  void ICACHE_RAM_ATTR interruptReboot(void) 
  {
    if (millis()-lwdTime >= LWD_TIMEOUT)
    {
      Sprintln("Rebooting...");
      flashBoardLed(1, 1000);
      ESP.restart();
    }
  }
  
  void initWDT()
  {
    Sprintln("Init Watchdog...");
    lwdTime = millis();
    lwdTicker.attach_ms(LWD_TIMEOUT, interruptReboot); // Attach lwdt interrupt service routine to ticker
  }
  
  void wdtReset()
  {
    // Reset timer feed dog
    lwdTime = millis();
  }
#endif
