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
      display.drawString(0 ,  0 , rssi); 
      display.drawString(0 , 15 , "Received " + packSize + " bytes");
      display.drawStringMaxWidth(0 , 26 , 128, oledString);
      display.drawString(0 , 40, "IP: " + WiFi.localIP().toString());
  
      Sprintln(rssi);
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

  rssiValue = String(LoRa.packetRssi(), DEC);
    
  rssi = "RSSI " + rssiValue;

  packet = EncryptDecrypt(packet, encryptKey);

  parseData(packet);

  Sprintln(packet);

  oledMillis = millis() + OledDelayOff;
}

void parseData(String data)
{
  int indexDelim[3]; // ALWAYS END WITH A DLEIMITER

  indexDelim[0] = data.indexOf(DELIMITER_LORA);
  indexDelim[1] = data.indexOf(DELIMITER_LORA, indexDelim[0]+1);
  indexDelim[2] = data.lastIndexOf(DELIMITER_LORA);

  Id = data.substring(0, indexDelim[0]);
  String Stts = data.substring(indexDelim[0]+1, indexDelim[1]);
  Vcc = data.substring(indexDelim[1]+1, indexDelim[2]);

  //mailPresent = Stts.equals("1");

  if (Stts.equals("1"))
  {
    delayMailPresent = millis();
    mailPresent = true;
  }
  else
  {
    if (millis()-delayMailPresent < mailPresentOff)
      return;
      
    mailPresent = false;
  }

  Bat = convertBat(Vcc);
  Sprintln(Bat);

  writeEEPROM();
}

//Convert battery voltage in 0-100%
String convertBat(String Voltage)
{
  float vccFloat = Vcc.toFloat() * 100.0;
  Sprintln(vccFloat);

  //3.15V = 0% / 3.28V = 100%
  return (String(constrain(map(int(vccFloat), 315, 328, 0, 100), 0, 100)));
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

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  EEPROM.begin(EEPROM_SIZE);
}

void readEEPROM()
{
  byte value_IDH  = EEPROM.read(address_IDH); 
  byte value_IDL  = EEPROM.read(address_IDL); 
  byte value_Stts = EEPROM.read(address_Stts);
  byte value_VccH = EEPROM.read(address_VccH);
  byte value_VccL = EEPROM.read(address_VccL);

  int value_ID = value_IDH * 256 + value_IDL;
  
  Id = String(value_ID);
  mailPresent = (value_Stts == 1) ? true : false;
  Vcc = String(float(value_VccH) + (float(value_VccL)/100.0));
  
  Sprintln("Read EEPROM: ");
  Sprintln(Id);
  Sprintln(mailPresent);
  Sprintln(Vcc);

  Bat = convertBat(Vcc);
}

void writeEEPROM()
{
  byte value_IDH  = highByte(Id.toInt());
  byte value_IDL  = lowByte(Id.toInt());
  byte value_Stts = mailPresent ? 1 : 0;
  byte value_VccH = int(Vcc.toFloat());
  byte value_VccL = String((Vcc.toFloat()-value_VccH)*100.0, 0).toInt();
  
  Sprintln("Write EEPROM: ");
  Sprintln(value_IDH);
  Sprintln(value_IDL);
  Sprintln(value_Stts);
  Sprintln(value_VccH);
  Sprintln(value_VccL);
  
  EEPROM.put(address_IDH, value_IDH);
  EEPROM.put(address_IDL, value_IDL);
  EEPROM.put(address_Stts, value_Stts);
  EEPROM.put(address_VccH, value_VccH);
  EEPROM.put(address_VccL, value_VccL);

  EEPROM.commit();

  readEEPROM();
}
