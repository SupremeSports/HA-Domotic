// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- TFT FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initTFT()
{
  Sprintln("Init TFT...");

  tft.begin();
  //tft.setRotation(6);
  drawSplashScreen();

  Sprintln("Init Touch...");
  touch.begin();
  #ifdef _debug
    Sprint("tftx ="); Sprint(tft.width()); Sprint(" tfty ="); Sprintln(tft.height());
  #endif

  //Beeper configuration
  Sprintln("Init Beeper...");
  #ifdef ENABLE_BEEPER
    ledcSetup(0,1E5,12);
    ledcAttachPin(21,0);
  #else
    Sprintln("Beeper Disabled!");
  #endif
}

void runTFT()
{
  //Detect touch event
  touchPressed = runTouch();

  //If screen has changed
  if (PrevMode != PMode)
  {
    drawMainScreen();
    drawTopRow();
    drawCodeScreen();
    drawOptionScreen();

    PrevMode = PMode;
  }
  //Update MAIN screen every second
  else if (PMode==PM_MAIN && ONS_1s)
  {  
    drawTopRow();
    drawModeIcons();
    updateRoomTemp();
  }
  else if (PMode==PM_CLEANING)
    Cleaning_processing();  //Run screen cleaning then restart
  
  if (PMode!=PM_OPTION)
    setOptionScreen = 1;

  //Set all timers when a touch is detected
  if (touchPressed)
  {
    ledMillis = millis() + ledDelayOff*1000;
    logMillis = millis() + logDelayOff*1000;
    returnMillis = millis() + returnToMain;
    
    sendTempMillis = millis() + SENDDATADELAY;
    sendFanMillis = millis() + SENDDATADELAY;
  }

  //Adjust logged in detection
  if (logDelayOff == 0)
    logged = true;      //Always logged if log off is disabled
  else if (millis() > logMillis && logged)
  {
    logged = false;     //Log off
    PMode = PM_MAIN;    //Return to MAIN screen after log off
    drawUpDnButtons();  //Hide temperature SET arrows
  }

  // LED screen timeout delay control
  // Option menu timeout after 30 seconds
  if (ledDelayOff > 0)
  {
    screenOff = (millis() > ledMillis);
    if (screenOff)
      PMode = PM_MAIN;
  }
  else
  {
    screenOff = false;
    if (millis() > returnMillis)
      PMode = PM_MAIN;
  }

  //Update screen with real data after delay data
  if (millis() > sendTempMillis && sendTempWait)
  {
      configTempSetpoint = bufferTempSetpoint != 0 ? bufferTempSetpoint : configTempSetpoint;
      sendTempWait = false;
      updateSetTemp();
      sendCommandTemp();
  }
  if (millis() > sendTempMillis && sendFanWait)
  {
      configFanMode = bufferFanMode != 0 ? bufferFanMode : configFanMode;
      sendFanWait = false;
      updateFanLevel();
      sendCommandFan();
  }
}

/********************************************************************//**
 * @brief     Processing for screen cleaning function
 * @param[in] None
 * @return    None
 *********************************************************************/
void Cleaning_processing()
{
  if (PMode != PM_CLEANING)
    return;

  PrevMode = PMode;
    
  // idle timer for screen cleaning
  if ((Timer_Cleaning % 10) == 0)
  {
    tft.fillRect(0,0, 100, 60, ILI9341_BLACK);
    tft.setCursor(10, 50);
    tft.setTextSize(0);
    tft.setFont(&FreeSansBold24pt7b);
    tft.print(Timer_Cleaning / 10);
    tft.setCursor(10, 100);
    tft.setTextSize(0);
    tft.setFont(&FreeSansBold9pt7b);
    tft.print("Thermostat will restart after cleaning...");
  }
  if (Timer_Cleaning)
    Timer_Cleaning -= 10;
  else
  {
    tft.fillScreen(ILI9341_BLACK);
    local_delay(500);
    ESP.restart(); //Restart after cleanup
    //drawOptionScreen();
    //PMode = PM_OPTION;
  }
}

//void drawPieSlice(int x, int y, int radius, int color, int startAngle, int EndAngle)
//{
//  for (int i=startAngle; i<EndAngle; i++)
//  {
//    double radians = i * PI / 180;
//    double px = x + radius * cos(radians);
//    double py = y + radius * sin(radians);
//    tft.drawPixel(px, py, color);
//  }
//}

void drawSplashScreen()
{
  tft.fillScreen(ILI9341_BLACK);
  
//  if (PMode != PM_BOOT);
//    return;
  

  #ifdef ENABLE_SPLASH
    tft.drawRGBBitmap(20,20, HA,200,201);
    local_delay(1);
    tft.drawRGBBitmap(20,250, Espressif,89,18);
    local_delay(1);
    tft.drawRGBBitmap(128,250, MQTT,92,21);
    local_delay(1);
    tft.drawRGBBitmap(68,280, GitHub,104,38);
    local_delay(1);
    tft.setFont(&FreeSansBold9pt7b);
    tft.setTextSize(0);
    tft.setTextColor(ILI9341_WHITE);
    //tft.setCursor(180, 310);
    tft.setCursor(5, 14);
    tft.print(version);
  #endif
  
}
// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TOUCH CONTROL -------------------------------------------
// ----------------------------------------------------------------------------------------------------
/********************************************************************//**
 * @brief     detects a touch event and converts touch data 
 * @param[in] None
 * @return    boolean (true = touch pressed, false = touch unpressed) 
 *********************************************************************/
bool runTouch()
{
  p = touch.getPoint();
  local_delay(1);
  p.x = map(p.x, TS_MINX, TS_MAXX, 320, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 240, 0);
  if (p.z > MINPRESSURE)
  {
    X = p.y; Y = p.x; //Vertical screen
    if (!touchPressed)
      detectButtons();

    return true;
  }
  
  return false;
}

/********************************************************************//**
 * @brief     detecting pressed buttons with the given touchscreen values
 * @param[in] None
 * @return    None
 *********************************************************************/
void detectButtons()
{
  Sprint("X = ");
  Sprint(X);
  Sprint(", Y = ");
  Sprint(Y);
  Sprintln();
  
  if (PMode == PM_MAIN)
    detectMainButtons();
  else if (PMode == PM_CODE)
    detectCodeButtons();
  else if (PMode == PM_OPTION)
    detectOptionsButtons();
}


void buttonAckTone()
{
  #ifndef ENABLE_BEEPER
    return;
  #endif

  //TODO
}
