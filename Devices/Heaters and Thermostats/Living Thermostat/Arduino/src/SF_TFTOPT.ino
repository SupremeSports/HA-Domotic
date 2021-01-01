// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- TFT OPTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
/********************************************************************//**
 * @brief     Drawing of the screen for Options menu
 * @param[in] None
 * @return    None
 *********************************************************************/
void drawOptionScreen()
{
  if (PMode != PM_OPTION)
    return;
  
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(0);

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);

  int yOffset = 0;
  int yOffsetSigns = 40;
  int yOffsetLine = 50;

  if (setOptionScreen == 1)
  {
    // Option #1 - LED OFF delay adjustment
    if (OPTION1)
    {
      yOffset = 20;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      tft.print("Screen Delay (0s=Never)");
      tft.setFont(&FreeSansBold24pt7b);
      tft.setCursor(30, yOffset+yOffsetSigns);
      tft.print("-");
      tft.setCursor(190, yOffset+yOffsetSigns);
      tft.print("+");
      tft.drawLine(5,yOffset+yOffsetLine,235,yOffset+yOffsetLine, ILI9341_WHITE);
      updateOptions(1, ledDelayOff);
    }
   
    // Option #2 - LOGOFF delay adjustment
    if (OPTION2)
    {
      yOffset = 90;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      tft.print("Logoff Delay (0s=Never)");
      tft.setFont(&FreeSansBold24pt7b);
      tft.setCursor(30, yOffset+yOffsetSigns);
      tft.print("-");
      tft.setCursor(190, yOffset+yOffsetSigns);
      tft.print("+");
      tft.drawLine(5,yOffset+yOffsetLine,235,yOffset+yOffsetLine, ILI9341_WHITE);
      updateOptions(2, logDelayOff);
    }
    
    // Option #3 - Dim Value adjustment
    if (OPTION3)
    {
      yOffset = 160;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      #ifdef ESP32
        tft.print("Dim Value (0-7)");
        tft.setFont(&FreeSansBold24pt7b);
        tft.setCursor(30, yOffset+yOffsetSigns);
        tft.print("-");
        tft.setCursor(190, yOffset+yOffsetSigns);
        tft.print("+");
        updateOptions(3, screenDimValue);
      #else
        tft.print("Dim Value (ESP32 Only)");
      #endif
      
      tft.drawLine(5,yOffset+yOffsetLine,235,yOffset+yOffsetLine, ILI9341_WHITE);
    }
  }
  else if (setOptionScreen == 2)
  {
    // Option #4 - BEEPER ON/OFF
    if (OPTION4)
    {
      yOffset = 20;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      tft.print("Beeper");
      tft.drawLine(5,yOffset+yOffsetLine,235,yOffset+yOffsetLine, ILI9341_WHITE);

      updateBooleans(4, beepON);
    }
    // Option #5 - BEEPER VOLUME
    if (OPTION5)
    {
      yOffset = 90;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      tft.print("Beeper Volume (0-7)");
      tft.setFont(&FreeSansBold24pt7b);
      tft.setCursor(30, yOffset+yOffsetSigns);
      tft.print("-");
      tft.setCursor(190, yOffset+yOffsetSigns);
      tft.print("+");
      tft.drawLine(5,yOffset+yOffsetLine,235,yOffset+yOffsetLine, ILI9341_WHITE);
      updateOptions(5, beepVolume);
    }
    // Option #6 - TODO
    if (OPTION6)
    {
      
    }
  }
  else if (setOptionScreen == 3)
  {
    // Option #7 - TODO
    if (OPTION7)
    {
      
    }
    // Option #8 - TODO
    if (OPTION8)
    {
      
    }
    // Option #9 - TODO
    if (OPTION9)
    {
      
    }
  }
  else if (setOptionScreen == 4)
  {
    // Option #10 - Informations
    if (OPTION10)
    {
      int extraLine = 20;
      yOffset = 20;
      tft.setFont(&FreeSansBold9pt7b);
      tft.setCursor(10, yOffset);
      tft.print("More information...");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"WiFi SSID: " + WiFi.SSID());

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"WiFi Rssi: " + rssi);

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"WiFi Percent: " + rssiPercent + "%");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"Voltage: " + voltage5V + "V");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"HVAC Air Temp: " + air_temp + "C");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"Air Temp: " + iRoom_temperature + "C");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"Air Humidity: " + iRoom_humidity + "%");

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"Version: " + version);

      yOffset += extraLine;
      tft.setCursor(20, yOffset);
      tft.print((String)"Date: " + date);
    }
  }

  //Page scroll arrows
  updateDots(setOptionScreen);
  
  // Clean/OK Button
  tft.setFont(&FreeSansBold12pt7b);
  tft.drawLine(5,260,235,260, ILI9341_WHITE);
  tft.drawLine(120,260,120,320, ILI9341_WHITE); //Vertical line
  tft.setCursor(18, 300);
  tft.print("CLEAN");
  tft.setCursor(160, 300);
  tft.print("OK");
}

void updateOptions(uint8_t option, uint8_t value)
{
  if (option<1 || option>3*MAXPAGE)
    return;
  else
  {
    option %= 3; //Max 3 options per screen
    if (option==0) option = 3;
  }
  
  int yOffset = (option-1)*70;

  int xOffset = 0;
  if (value < 10)
    xOffset = (240/2)-12;
  else if (value < 100)
    xOffset = (240/2)-25;
  else
    xOffset = (240/2)-40;
  
  tft.fillRect(60, 30+yOffset, 120, 40, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(xOffset, 65+yOffset);
  tft.print(value);
}

void updateBooleans(uint8_t option, bool isON)
{
  if (option<1 || option>3*MAXPAGE)
    return;
  else
  {
    option %= 3; //Max 3 options per screen
    if (option==0) option = 3;
  }

  int xOffset = (240/4)-45;
  int yOffset = (option-1)*70;

  //OFF
  tft.fillRect(0, 30+yOffset, 120, 40, !isON ? ILI9341_WHITE : ILI9341_BLACK);
  tft.setTextColor(!isON ? ILI9341_BLACK : ILI9341_WHITE);
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(xOffset, 65+yOffset);
  tft.print("OFF");

  //ON
  xOffset = (240/4)-40 + 240/2;
  
  tft.fillRect(120, 30+yOffset, 120, 40, isON ? ILI9341_WHITE : ILI9341_BLACK);
  tft.setTextColor(isON ? ILI9341_BLACK : ILI9341_WHITE);
  tft.setFont(&FreeSansBold24pt7b);
  tft.setCursor(xOffset, 65+yOffset);
  tft.print("ON");

  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
}

void updateDots(uint8_t page)
{
  int yOffset = 245;
  int xSpacing = 15;
  int radius = 5;
  int xStart = (240/2) - ((MAXPAGE-1)*xSpacing)/2;
  
  for (int i=0; i<MAXPAGE; i++)
    tft.fillCircle((xStart+(i*xSpacing)), yOffset, radius, i+1==page ? ILI9341_WHITE : ILI9341_ULTRA_DARKGREY);

  tft.fillTriangle(5,235,25,225,25,245, setOptionScreen>1 ? ILI9341_WHITE : ILI9341_BLACK);
  tft.fillTriangle(235,235,215,225,215,245, setOptionScreen < MAXPAGE ? ILI9341_WHITE : ILI9341_BLACK);
}


// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TOUCH CONTROL -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void detectOptionsButtons()
{
  // OPTIONS screen
  if (PMode != PM_OPTION)
    return;

  // Left buttons
  if (X<110)
  {
    if (Y<70)   
    {
      if (OPTION1 && setOptionScreen==1)
      {
        // Option #1 - LED OFF delay adjustment
        if (ledDelayOff > 0) ledDelayOff--;
        else ledDelayOff=120; //Maximum 2min
        updateOptions(1, ledDelayOff);
      }
      else if (OPTION4 && setOptionScreen==2)
      {
        beepON = false;

        updateBooleans(4, beepON);
      }
      else if (OPTION7 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<140)
    {
      if (OPTION2 && setOptionScreen==1)
      {
        // Option #2 - LOGOFF delay adjustment
        if (logDelayOff > 0) logDelayOff--;
        else logDelayOff=120; //Maximum 2min
        updateOptions(2, logDelayOff);
      }
      else if (OPTION5 && setOptionScreen==2)
      {
        // Option #5 - BEEPER VOLUME adjustment
        if (beepVolume > 0) beepVolume--;
        else beepVolume=7;
        updateOptions(5, beepVolume);
      }
      else if (OPTION8 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<210)
    {
      if (OPTION3 && setOptionScreen==1)
      {
        // Option #3 - Dim value adjustment
        if (screenDimValue > 0) screenDimValue--;
        else screenDimValue=7;
        #ifdef ESP32
          updateOptions(3, screenDimValue);
        #endif
      }
      else if (OPTION6 && setOptionScreen==2)
      {
        //TODO
      }
      else if (OPTION9 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<255)
    {
      if (setOptionScreen > 1)
      {
        setOptionScreen--;
        drawOptionScreen();
      }
    }
    // Clean/Restart Button
    else if (Y>265)
    {
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.setFont(&FreeSansBold24pt7b);
      Timer_Cleaning = 250;
      PMode = PM_CLEANING;
    }
  }

  // Right buttons
  if(X>130)
  {
    if (Y<70)   
    {
      if (OPTION1 && setOptionScreen==1)
      {
        // Option #1 - LED OFF delay adjustment
        if (ledDelayOff < 120) ledDelayOff++; //Maximum 2min
        else ledDelayOff=0;
        updateOptions(1, ledDelayOff);
      }
      else if (OPTION4 && setOptionScreen==2)
      {
        beepON = true;

        updateBooleans(4, beepON);
      }
      else if (OPTION7 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<140)
    {
      if (OPTION2 && setOptionScreen==1)
      {
        // Option #2 - LOGOFF delay adjustment
        if (logDelayOff < 120) logDelayOff++; //Maximum 2min
        else logDelayOff=0;
        updateOptions(2, logDelayOff);
      }
      else if (OPTION5 && setOptionScreen==2)
      {
        // Option #5 - BEEPER VOLUME adjustment
        if (beepVolume < 7) beepVolume++;
        else beepVolume=0;
        updateOptions(5, beepVolume);
      }
      else if (OPTION8 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<210)
    {
      if (OPTION3 && setOptionScreen==1)
      {
        // Option #3 - Dim Value adjustment
        if (screenDimValue < 7) screenDimValue++;
        else screenDimValue=0;
        #ifdef ESP32
          updateOptions(3, screenDimValue);
        #endif
      }
      else if (OPTION6 && setOptionScreen==2)
      {
        //TODO
      }
      else if (OPTION9 && setOptionScreen==3)
      {
        //TODO
      }
    }
    else if (Y<255)
    {
      if (setOptionScreen < MAXPAGE)
      {
        setOptionScreen++;
        drawOptionScreen();
      }
    }
    // OK Button
    else if (Y>265)
    {
      Thermostat_mode = BOOT;
      drawMainScreen();
      PMode = PM_MAIN;
    }
  }
}
