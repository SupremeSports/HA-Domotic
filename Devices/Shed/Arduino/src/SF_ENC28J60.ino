// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ETHERNET SETUP -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEthernet()
{
 
  Sprint("Init Ethernet...");
  //Start Ethernet communication
  Ethernet.begin(mac, ip, dnsIp, gateway, subnet);
 
  //Wait for Ethernet to connect
  flashBoardLed(100, 5);
  wdtReset();

  Sprintln("Connected!");

  networkActive = true;

  lastSecond = millis()-10000;
}

bool checkNetwork()
{
  Ethernet.maintain();
  
  bool networkActive = (Ethernet.linkStatus() == LinkON);
  if (!networkActive)
    initEthernet();

  return networkActive;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- OTA SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initOTA()
{
  #ifdef ENABLE_OTA
    Sprintln("Init OTA...");
    Sprintln(Ethernet.localIP());
    
    // start the OTEthernet library with internal (flash) based storage
    ArduinoOTA.begin(Ethernet.localIP(), "arduino", "password", InternalStorage);
  #else
    Sprintln("OTA Disabled...");
  #endif
}

void runOTA()
{
  #ifdef ENABLE_OTA
    ArduinoOTA.poll();
  #endif
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWDT()
{
  if (ENABLE_WDT)
  {
    wdt_disable();
    wdt_enable(WDTO_8S);
  }
}

void wdtReset()
{
  // Reset timer feed dog
  wdt_reset();
}
