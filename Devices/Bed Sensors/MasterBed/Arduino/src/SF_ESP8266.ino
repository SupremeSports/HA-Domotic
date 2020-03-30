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
  
  ArduinoOTA.setPort(8266);
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
void ICACHE_RAM_ATTR interruptReboot(void) 
{
  if (millis()-lwdTime >= LWD_TIMEOUT)
  {
    Sprintln(json_resetReboot);
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

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
}

void readEEPROM()
{
  byte value_presetH  = EEPROM.read(address_presetH);
  byte value_presetL  = EEPROM.read(address_presetL);
  byte value_presetS  = EEPROM.read(address_presetS);

  loadcellCalib = value_presetH * 256 + value_presetL;

  loadcellCalib *= (value_presetS==1 ? -1 : 1);

  offsetScale = EEPROMReadlong(address_offset);
  scale.set_offset(offsetScale);
  
  Sprintln("Read EEPROM: ");
  Sprintln(loadcellCalib);
}

void writeEEPROM()
{
  int calib = abs(loadcellCalib);
  
  byte value_presetH  = highByte(calib);
  byte value_presetL  = lowByte(calib);
  byte value_presetS  = (loadcellCalib<0 ? 1 : 0);
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_presetH);
  Sprintln(value_presetL);
  Sprintln(value_presetS);
  
  EEPROM.put(address_presetH, value_presetH);
  EEPROM.put(address_presetL, value_presetL);
  EEPROM.put(address_presetS, value_presetS);

  offsetScale = scale.get_offset();
  EEPROMWritelong(address_offset, offsetScale);

  EEPROM.commit();

  readEEPROM();
}

long EEPROMReadlong(long address)
{
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  Sprint(four);
  Sprint(" : ");
  Sprint(three);
  Sprint(" : ");
  Sprint(two);
  Sprint(" : ");
  Sprintln(one);
 
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROMWritelong(int address, long value)
{
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
 
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);

  Sprint(four);
  Sprint(" : ");
  Sprint(three);
  Sprint(" : ");
  Sprint(two);
  Sprint(" : ");
  Sprintln(one);
}
