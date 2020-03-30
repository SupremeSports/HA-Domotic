// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ETHERNET SETUP -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEthernet()
{
 
  Sprintln("Init Ethernet...");
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

//TODO

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWDT()
{
  if (ENABLE_WDT)
    wdt_enable(WDTO_8S);
}

void wdtReset()
{
  // Reset timer feed dog
  wdt_reset();
}
