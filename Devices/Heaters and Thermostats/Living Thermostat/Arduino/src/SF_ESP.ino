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
  ArduinoOTA.setPassword(password[0]);

  ArduinoOTA.onStart([]()
  {
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
  bool screenOn = false;
  Sprintln("Check restart reason...");
  Sprint("reset  reason: ");
  
  #ifdef ESP32
    int Core0 = rtc_get_reset_reason(0);
    int Core1 = rtc_get_reset_reason(1);

    Sprintln();
    Sprint("  Core0: ");
    resetReason(Core0);
    Sprint("  Core1: ");
    resetReason(Core1);

    screenOn = (Core0==1 || Core0==16); //Temporarily added 16 as this code always pops
  #elif ESP8266
    struct  rst_info  *rtc_info = system_get_rst_info();
    Sprint("reset  reason: ");
    Sprintln(rtc_info->reason);

    screenOn = (rtc_info->reason == 6)
  #endif

  //Don't light screen up if it's not booting from a power up event
  if (screenOn)
  {
    screenOff = false;
    ledMillis = millis() + ledDelayOff*1000;
    PMode = PM_BOOT;
    Sprintln("Normal Boot");
  }
  else
  {
    screenOff = true;
    ledMillis = millis()-1;
    //PMode = PM_MAIN;
    Sprintln("WDT Boot");
  }
}

void resetReason(int coreReason)
{
  Sprint(coreReason);
  Sprint(": ");
 
  switch (coreReason)
  {
    case 1 : Sprintln ("POWERON_RESET");break;          /**<1,  Vbat power on reset*/
    case 3 : Sprintln ("SW_RESET");break;               /**<3,  Software reset digital core*/
    case 4 : Sprintln ("OWDT_RESET");break;             /**<4,  Legacy watch dog reset digital core*/
    case 5 : Sprintln ("DEEPSLEEP_RESET");break;        /**<5,  Deep Sleep reset digital core*/
    case 6 : Sprintln ("SDIO_RESET");break;             /**<6,  Reset by SLC module, reset digital core*/
    case 7 : Sprintln ("TG0WDT_SYS_RESET");break;       /**<7,  Timer Group0 Watch dog reset digital core*/
    case 8 : Sprintln ("TG1WDT_SYS_RESET");break;       /**<8,  Timer Group1 Watch dog reset digital core*/
    case 9 : Sprintln ("RTCWDT_SYS_RESET");break;       /**<9,  RTC Watch dog Reset digital core*/
    case 10 : Sprintln ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : Sprintln ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : Sprintln ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : Sprintln ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : Sprintln ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : Sprintln ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : Sprintln ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : Sprintln ("NO_MEAN");
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