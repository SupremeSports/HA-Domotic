// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- INIT FUNCTIONS --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initOLED()
{
  #ifndef ENABLEOLED
    Sprintln("OLED Disabled...");
    return;
  #else
    Sprintln("Init OLED...");
    digitalWrite(resetOledPin, LOW);    // set GPIO16 low to reset OLED
    local_delay(50); 
    digitalWrite(resetOledPin, HIGH);   // while OLED is running, must set GPIO16 in high
  
    local_delay(100);
    
    display.init();
    display.flipScreenVertically();  
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0 ,  0 , "Starting LoRa Receiver!");
    display.drawString(0 ,  15 , "Please Wait...");
    display.display();
  #endif
}

void initLora()
{
  #ifndef ENABLELORA
    Sprintln("LoRa Disabled...");
    return;
  #endif
  
  Sprint("Init LoRa...");
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);  
  if (!LoRa.begin(BAND))
  {
    Sprintln("Failed!");
    //TODO - while (1);
  }
  else
    Sprintln("Connected!");

  LoRa.setSpreadingFactor(12);           // Ranges from 6-12,default 7 see API docs

  LoRa.receive();

  blockNoMailTrigger = false;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OLED DISPLAY --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void oledUpdate()
{
  #ifndef ENABLEOLED
    return;
  #else
    display.clear();
    if (!digitalRead(powerOledPin) || (oledMillis >= millis()))
    {
      String oledString = Id + " - ";
      oledString += (mailPresent ? NEWMAIL : NOMAIL);
      oledString += " - " + Vcc + "V";
    
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_10);
      display.drawString(0 ,  0 , rssiStr); 
      display.drawString(0 , 15 , "Received " + packSize + " bytes");
      display.drawStringMaxWidth(0 , 26 , 128, oledString);
      display.drawString(0 , 40, "IP: " + WiFi.localIP().toString());
  
      Sprintln(rssiStr);
      Sprintln(WiFi.localIP());
    }
    display.display();
  #endif
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- LoRa Callback --------------------------------------------
// ----------------------------------------------------------------------------------------------------
boolean loraRead()
{
  #ifndef ENABLELORA
    return false;
  #endif

  if (millis() >= delayMailPresent)
    blockNoMailTrigger = false;
  
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    loraCallback(packetSize);
    flashBoardLed(100, 1);
    return true;
  }
  return false;
}

void loraCallback(int packetSize)
{
  packet = "";
  packSize = String(packetSize,DEC);
  
  for (int i = 0; i < packetSize; i++)
    packet += (char) LoRa.read();

  newStatusForced = false;

  Rssi = String(LoRa.packetRssi(), DEC);
    
  rssiStr = "RSSI " + Rssi;

  packet = EncryptDecrypt(packet, encryptKey);

  parseData(packet);
  Sprintln(packet);

  oledMillis = millis() + OledDelayOff;
}

void parseData(String data)
{
  int indexDelim[3]; //ALWAYS END WITH A DELIMITER ==> ID:1 or 0:Vcc:

  indexDelim[0] = data.indexOf(DELIMITER_LORA);
  indexDelim[1] = data.indexOf(DELIMITER_LORA, indexDelim[0]+1);
  indexDelim[2] = data.lastIndexOf(DELIMITER_LORA);

  Id = data.substring(0, indexDelim[0]);
  String Stts = data.substring(indexDelim[0]+1, indexDelim[1]);
  Vcc = data.substring(indexDelim[1]+1, indexDelim[2]);

  if (!enableFiveMinuteDelay)
    mailPresent = Stts.equals("1");
  else
  {
    if (Stts.equals("1"))
    {
      delayMailPresent = millis()+mailPresentOff;
      blockNoMailTrigger = true;
      mailPresent = true;
  
      Sprintln("NEW MAIL");
    }
    else if (!blockNoMailTrigger)
    {
      delayMailPresent = 0;
      blockNoMailTrigger = false;
      mailPresent = false;
  
      Sprintln("NO MAIL");
    }
    else
    {
      int timeLeft = delayMailPresent - millis();
      Sprint("Delay not expired: ");
      Sprint(timeLeft);
      Sprintln("ms left!!!");
    }
  }

  Bat = convertBat(Vcc);

  writeEEPROM();

  newPostalStatus = true;
}

//Convert battery voltage in 0-100%
String convertBat(String Voltage)
{
  float vccInt = Vcc.toFloat() * 100.0;

  //3.18V = 0% / 3.32V = 100%
  return (String(constrain(map(int(vccInt), 318, 332, 0, 100), 0, 100)));
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ENCRYPT/DECRYPT -----------------------------------------
// ----------------------------------------------------------------------------------------------------
String EncryptDecrypt(String szPlainText, int szEncryptionKey)  
{  
  String szInputStringBuild = szPlainText;
  String szOutStringBuild = "";
  char Textch;
  for (int iCount = 0; iCount < szPlainText.length(); iCount++)
  {
    Textch = szInputStringBuild[iCount];
    Textch = (char)(Textch ^ szEncryptionKey);
    szOutStringBuild += Textch;
  }  
  
  return szOutStringBuild;
}
