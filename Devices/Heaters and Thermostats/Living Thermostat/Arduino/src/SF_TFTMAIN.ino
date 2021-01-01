// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- TFT MAIN ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
/********************************************************************//**
 * @brief     Drawing of the main program screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void drawMainScreen()
{
  if (PMode != PM_MAIN)
    return;

  if (PrevMode != PMode)
  {
    if (PrevMode != PM_SLEEP)
      writeEEPROM();
    Thermostat_mode = BOOT;
  }

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(0);

  // Draw temperature up/dwn buttons
  drawUpDnButtons();

  // Draw FAN icons & level
  drawFanIcons();
  updateFanLevel();

  // Draw set temperature and circles
  updateSetTemp();

  // Draw different icons
  drawTopRow();
  drawModeIcons();
}

/********************************************************************//**
 * @brief     update of the value for set temperature on the screen
 *            (in the big colored circle)
 * @param[in] None
 * @return    None
 *********************************************************************/
void updateRoomTemp()
{
  if (PMode != PM_MAIN)
    return;

  bool forceUpdate = PrevMode != PM_MAIN;

  bool updateTemp = (iRoom_temperature != iRoom_prevTemp || forceUpdate);
  bool updateHum = (iRoom_humidity != iRoom_prevHum || forceUpdate);
  iRoom_prevTemp = iRoom_temperature;
  iRoom_prevHum = iRoom_humidity;
    
  int16_t x1, y1;
  uint16_t w, h;
  String curValue = "";
  int str_len = 0;

  tft.setTextColor(ILI9341_WHITE, ILI9341_ULTRA_DARKGREY);
  tft.setFont(&FreeSansBold12pt7b);
  //tft.fillRect(11, 168, 49, 47, ILI9341_ULTRA_DARKGREY);

  if (updateTemp)
  {
    if (iRoom_humidity == -1.0)
      curValue = "--.-";
    else if (iRoom_temperature > -10 && iRoom_temperature < 100)
      curValue = String(iRoom_temperature, 1);
    else
      curValue = String(iRoom_temperature, 0);
  
    str_len = curValue.length() + 1;
    char char_arrayTemp[str_len];
    curValue.toCharArray(char_arrayTemp, str_len);
    tft.fillRect(11, 169, 50, 20, ILI9341_ULTRA_DARKGREY);
    tft.getTextBounds(char_arrayTemp, 50, 190, &x1, &y1, &w, &h);
    tft.setCursor(57 - w, 187);
    tft.print(char_arrayTemp);

    updateCircleColor();
  }

  if (updateHum)
  {
    if (iRoom_humidity == -1.0)
      curValue = "--.-";
    else if (iRoom_humidity > -10 && iRoom_humidity < 100)
      curValue = String(iRoom_humidity, 1);
    else
      curValue = String(iRoom_humidity, 0);
      
    str_len = curValue.length() + 1;
    char char_arrayHum[str_len];
    curValue.toCharArray(char_arrayHum, str_len);
    tft.fillRect(11, 192, 50, 20, ILI9341_ULTRA_DARKGREY);
    tft.getTextBounds(char_arrayHum, 50, 190, &x1, &y1, &w, &h);
    tft.setCursor(57 - w, 210);
    tft.print(char_arrayHum);
  }
}

/********************************************************************//**
 * @brief     update of the value for room temperature on the screen
 *            (in the small grey circle)
 * @param[in] None
 * @return    None
 *********************************************************************/
void updateSetTemp()
{
  if (PMode != PM_MAIN)
    return;
    
  int16_t x1, y1;
  uint16_t w, h;
  String tempValue = sendTempWait ? String(bufferTempSetpoint) : String(configTempSetpoint);
  String curValue = stateHVAC ? tempValue : "---";
  int str_len =  curValue.length() + 1;
  char char_array[str_len];
  curValue.toCharArray(char_array, str_len);
  tft.fillRect(70, 96, 60, 50, ILI9341_BLACK);
  tft.setTextColor(sendTempWait && stateHVAC ? ILI9341_RED : ILI9341_WHITE);
  tft.setFont(&FreeSansBold24pt7b);
  tft.getTextBounds(char_array, 80, 130, &x1, &y1, &w, &h);
  tft.setCursor(123 - w, 130);
  tft.print(char_array);

  updateCircleColor();
}

/********************************************************************//**
 * @brief     drawing of the circles in main screen including the value 
 *            of room temperature
 * @param[in] None
 * @return    None
 *********************************************************************/
void drawCircles()
{
  if (PMode != PM_MAIN)
    return;
    
  //Draw big circle 
  int i;
  bool updateCircle = false;
  uint16_t colorSet = 0;
  
  if (ILI9341_COLOR != 0)
  {
    if (PrevMode != PMode)
    {
      // Fixed Color
      colorSet = ILI9341_COLOR;
      updateCircle = true;
    }
  }
  else if (Thermostat_mode == HEATING)
  {
    // Heating - RED
    colorSet = ILI9341_RED;
    updateCircle = true;
  }
  else if (Thermostat_mode == COOLING)
  {
    // Cooling - BLUE
    colorSet = ILI9341_BLUE;
    updateCircle = true;
  }
  else
  {
    // Temperature OK      
    colorSet = ILI9341_GREEN;
    updateCircle = true;
  }

  if (updateCircle)
  {
    for(i=0; i<10; i++)
      tft.drawCircle(120, 120, 90 + i, colorSet);
  }
  else
    return;

  //Draw °C in big circle
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&FreeSansBold9pt7b);
  tft.setCursor(130, 100);
  tft.print("o");
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(140, 130);
  tft.print(tempF ? "F" : "C");

  //Draw small circle
  //tft.fillCircle(60, 200, 40, ILI9341_ULTRA_DARKGREY);
  tft.fillCircle(50, 190, 45, ILI9341_ULTRA_DARKGREY);

  //Draw °C and % in small circle
  tft.setTextColor(ILI9341_WHITE, ILI9341_ULTRA_DARKGREY);
  tft.setFont(&FreeSansBold12pt7b);
  tft.drawCircle(65,170, 2, ILI9341_WHITE);
  tft.drawCircle(65,170, 3, ILI9341_WHITE);
  tft.setCursor(68, 187);
  tft.print(tempF ? "F" : "C");
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(63, 210);
  tft.print("%");
  
  updateRoomTemp();
}

/********************************************************************//**
 * @brief     update of the color of the big circle according the 
 *            difference between set and room temperature 
 * @param[in] None
 * @return    None
 *********************************************************************/
void updateCircleColor()
{
  // Temperature ok 
  if ((iRoom_temperature > configTempSetpoint-1 && iRoom_temperature < configTempSetpoint+1 && Thermostat_mode != TEMP_OK) || Thermostat_mode == BOOT)
  {
    Thermostat_mode = TEMP_OK;
    drawCircles();
  }
  
  // HEATING 
  else if ((iRoom_temperature < configTempSetpoint-1) && (Thermostat_mode != HEATING))
  {
    Thermostat_mode = HEATING;
    drawCircles();
  }

  // COOLING 
  else if ((iRoom_temperature > configTempSetpoint+1) && (Thermostat_mode != COOLING))
  {
    Thermostat_mode = COOLING;
    drawCircles();
  }
}

/********************************************************************//**
 * @brief     drawing of the both buttons for setting temperature up 
 *            and down
 * @param[in] None
 * @return    None
 * tft.fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 * x0：x coordinate of the apex
 * y0：y coordinate of the apex
 * x1：x coordinate of the bottom left point
 * y1：y coordinate of the bottom left point
 * x2：x coordinate of the bottom right point
 * y2：y coordinate of the bottom right point
 *********************************************************************/
void drawUpDnButtons()
{
  if (PMode != PM_MAIN)
    return;

  bool showArrows = logged;
    
  //UP button 
  tft.fillTriangle(120,50,135,70,105,70, showArrows ? ILI9341_WHITE : ILI9341_BLACK);
    
  //DOWN button
  tft.fillTriangle(120,180,135,160,105,160, showArrows ? ILI9341_WHITE : ILI9341_BLACK);
}

/********************************************************************//**
 * @brief     drawing of the fan icons in main screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void drawFanIcons()
{
  if (PMode != PM_MAIN)
    return;
    
  //tft.drawRGBBitmap(10,290, fan_blue_24,24,24);
  //tft.drawRGBBitmap(200,282, fan_blue_32,32,32);
  tft.drawBitmap(10,290,fan_24_icon, 24, 24, ILI9341_LIGHTBLUE);
  tft.drawBitmap(200,282,fan_32_icon, 32, 32, ILI9341_LIGHTBLUE);
  
  //tft.drawRGBBitmap(5,35, gear_icon, 24,24);
  tft.drawBitmap(5,35,gear_24_icon, 24, 24, ILI9341_ULTRA_DARKGREY);
}

/********************************************************************//**
 * @brief     drawing of the fan level in main screen
 * @param[in] None
 * @return    None
 *********************************************************************/
void updateFanLevel()
{
  if (PMode != PM_MAIN)
    return;
  
  uint16_t x0 = 50;
  uint16_t y0 = 312;

  uint8_t ilevel = sendFanWait ? bufferFanMode : configFanMode;
  ilevel = constrain(ilevel+1, 0, 5);
  
  unsigned char i;
  if (!hvacAlive)
  {
    for(i=0; i<5; i++)
      tft.fillRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), ILI9341_BLACK);
  }
  else
  {
    for(i=0; i<5; i++)
    {
      if (i < ilevel)
        tft.fillRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), sendFanWait ? ILI9341_RED : ILI9341_BLUE);
      else
      {
        tft.fillRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), ILI9341_BLACK);
        tft.drawRect(x0 + (30*i), y0- 10 -(i*8), 20, 10 + (i*8), ILI9341_WHITE);
      }
    }
  }
}

/********************************************************************//**
 * @brief     drawing of the top row icons and clock
 * @param[in] None
 * @return    None
 * ICONS MUST BE 16px x 16px
 *********************************************************************/
void drawTopRow()
{
  if (PMode != PM_MAIN)
    return;

  bool forceUpdate = PrevMode != PM_MAIN;
    
  //Clear icons
  //tft.fillRect(0, 0, 150, 20, ILI9341_BLACK);

  int xIcons = 5;     //Start at 5 pixels from left
  int yIcons = 3;     //Start at 3 pixels from top

  int iconWdth = 16;  //ICONS MUST BE 16px x 16px 
  int xSpace = 20;    //Spacing between icons

  bool updateCircle = false;
  uint16_t colorSet = 0;

  //Wifi connectivity
  if (!networkActive)
    tft.drawBitmap(xIcons,yIcons,wifi_full, iconWdth, iconWdth, ILI9341_ULTRA_DARKGREY);
  else
  {
    tft.drawBitmap(xIcons,yIcons,wifi_full, iconWdth, iconWdth, ILI9341_BLACK);
    if (rssiPercent>=60)
      tft.drawBitmap(xIcons,yIcons,wifi_full, iconWdth, iconWdth, ILI9341_WHITE);
    else if (rssiPercent>55)  
      tft.drawBitmap(xIcons,yIcons,wifi_med, iconWdth, iconWdth, ILI9341_WHITE);
    else
      tft.drawBitmap(xIcons,yIcons,wifi_low, iconWdth, iconWdth, ILI9341_WHITE);
  }
  xIcons += xSpace; //Wifi icon always shown

  //Power ON
  tft.drawBitmap(xIcons, yIcons, flash_icon, iconWdth, iconWdth, configStateOn ? ILI9341_YELLOW : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;

  //Eco mode
  tft.drawBitmap(xIcons, yIcons, leaf_icon, iconWdth, iconWdth, eco ? ILI9341_DARKGREEN : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;

  // User logged in
  tft.drawBitmap(xIcons, yIcons, user_icon, iconWdth, iconWdth, logged ? ILI9341_WHITE : ILI9341_BLACK);
  xIcons += xSpace;

  // Notifications
  tft.drawBitmap(xIcons, yIcons, bell_icon, iconWdth, iconWdth, notif ? ILI9341_WHITE : ILI9341_BLACK);
  xIcons += xSpace;

  if (HourMinute == prevHourMinute && !forceUpdate)
    return;
  prevHourMinute = HourMinute;
  
  //Clear clock
  tft.fillRect(160, 0, 80, 25, ILI9341_BLACK);

  // Clock
  tft.setTextColor(ILI9341_WHITE);
  tft.setFont(&FreeSansBold12pt7b);
  tft.setCursor(178, 20);
  tft.print(HourMinute);
}

/********************************************************************//**
 * @brief     drawing of the top row icons and clock
 * @param[in] None
 * @return    None
 * ICONS MUST BE 25px x 25px
 *********************************************************************/
void drawModeIcons()
{
  if (PMode != PM_MAIN)
    return;

  int iconQty = 5;    // Max of 6 icons can fit
  int iconWdth = 25;  //ICONS MUST BE 25px x 25px

  int xIcons = 10;    //Leave 10 pixels on each side
  int yIcons = 236;   //Start at 236 pixels from top

  int xSpace = ((240 - 2*xIcons - iconQty*iconWdth) / (iconQty-1)) + iconWdth;  //Dynamic spacing between icons
    
  bool refreshIcon = (runModes[configRunMode] == "heat") && hvacAlive;
  tft.drawBitmap(xIcons,yIcons,ha_heat, iconWdth, iconWdth, refreshIcon ? ILI9341_ORANGE : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;

  refreshIcon = (runModes[configRunMode] == "cool") && hvacAlive;
  tft.drawBitmap(xIcons,yIcons,ha_cool, iconWdth, iconWdth, refreshIcon ? ILI9341_CYAN : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;
  
  refreshIcon = (runModes[configRunMode] == "dry") && hvacAlive;
  tft.drawBitmap(xIcons,yIcons,ha_dry, iconWdth, iconWdth, refreshIcon ? ILI9341_YELLOW : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;

  refreshIcon = (runModes[configRunMode] == "fan_only") && hvacAlive;
  tft.drawBitmap(xIcons,yIcons,ha_fan, iconWdth, iconWdth, refreshIcon ? ILI9341_PURPLE : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;
  
  refreshIcon = (runModes[configRunMode] == "off") && hvacAlive;
  tft.drawBitmap(xIcons,yIcons,ha_power, iconWdth, iconWdth, refreshIcon ? ILI9341_RED : ILI9341_ULTRA_DARKGREY);
  xIcons += xSpace;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TOUCH CONTROL -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void detectMainButtons()
{
  // MAIN screen

  // GEAR button
  if ((X<60) && (Y<75))
  {
    if (logged)
    {
      drawOptionScreen();
      PMode = PM_OPTION;
    }
    else
    {
      drawCodeScreen();
      PMode = PM_CODE;
    }
  }

  //If not logged, no other buttons are available
  if (!logged)
    return;

  //UP & DOWN buttons
  if (X>70 && X<170)
  {
    // button UP
    if (Y>25 && Y<90)
    {
      if (!sendTempWait)
        bufferTempSetpoint = configTempSetpoint;
      if (bufferTempSetpoint < MAX_TEMPERATURE) 
        bufferTempSetpoint++;
        
      #ifdef WAITBEFORESENDING
        sendTempWait = (bufferTempSetpoint != configTempSetpoint);
      #else
        configTempSetpoint = bufferTempSetpoint;
        sendTempWait = false;
        sendCommandTemp();
      #endif

      updateSetTemp();
    }
    
    // button DWN
    if (Y>120 && Y<200)
    {
      if (!sendTempWait)
        bufferTempSetpoint = configTempSetpoint;
      if (bufferTempSetpoint > MIN_TEMPERATURE)
        bufferTempSetpoint--;
        
      #ifdef WAITBEFORESENDING
        sendTempWait = (bufferTempSetpoint != configTempSetpoint);
      #else
        configTempSetpoint = bufferTempSetpoint;
        sendTempWait = false;
        sendCommandTemp();
      #endif

      updateSetTemp();
    }
  }
  
  //FAN buttons
  if (Y>270)
  {
    // button FAN MAX
    if (X>180)
    {
      if (!sendFanWait)
        bufferFanMode = configFanMode;
      if (bufferFanMode < FANMODES_MAX)
        bufferFanMode++;

      #ifdef WAITBEFORESENDING
        sendFanWait = (bufferFanMode != configFanMode);
      #else
        configFanMode = bufferFanMode;
        sendFanWait = false;
        sendCommandFan();
      #endif

      updateFanLevel();
    }
    
    // button FAN MIN
    if (X<60)
    {
      if (!sendFanWait)
        bufferFanMode = configFanMode;
      if (bufferFanMode > FANMODES_MIN)
        bufferFanMode--;
      
      #ifdef WAITBEFORESENDING
        sendFanWait = (bufferFanMode != configFanMode);
      #else
        configFanMode = bufferFanMode;
        sendFanWait = false;
        sendCommandFan();
      #endif

      updateFanLevel();
    }
  }

  //MODES buttons
  if (Y>220 && Y<260)
  {
    int iconQty = 5;
    int iconWdth = 240 / iconQty;
  
    int xIcon1 = 0;
    int xIcon2 = xIcon1 + iconWdth;   //Start at 5 pixels from left

    for (int i=RUNMODES_MIN; i<=RUNMODES_MAX; i++)
    {
      if (X<xIcon2 && X>xIcon1)
      {
        configRunMode = i;

        Sprint("Mode set to: ");
        Sprintln(runModes[configRunMode]);

        if (runModes[i]=="off")
          sendCommandPower();
        sendCommandMode();
    
        break;
      }

      xIcon1 += iconWdth;
      xIcon2 += iconWdth;
    }
  }
}
