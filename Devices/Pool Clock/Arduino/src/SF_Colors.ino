// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- COLORS FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void autoColorChanging()
{
  //Switch what we are showing every 5 seconds
  if (millis() > nextSwitch && colorAutoSwitch)
  {
    nextSwitch = millis() + 5000;
    displayFeature = (displayFeature + 1) % 7; //Modulo number of cases (0 to 6 = 7)
    nextRainbow = millis()-1;

    rainbowIndex = 0;

    Sprint("Display feature: ");
    Sprintln(displayFeature);
  }

  //Get string length to determine if we have a string to display
  int lenTime = timeTextString.length();
  int lenTemp = tempTextString.length();

  //Display time for 5 second even if we have a string to display
  if (Second < 10 && localTimeValid)
  {
    lenTime = 0;
    lenTemp = 0;
    rollingTimeTextIndex = 1;
    rollingTempTextIndex = 1;
  }

  #ifndef ENABLE_TEXTSTRINGS
    lenTime = 0;
    lenTemp = 0;
  #endif

  if (nextRainbow < millis())
  {
    if (initDisplays)
      colorChangingSequences("8.8.8.8.8.8.", "8.8.8.8.8.8.8.8."); //Update color sequence
    else
      colorChangingSequences((lenTime == 0) ? timeString : timeTextString, (lenTemp == 0) ? waterString+tempString : tempTextString); //Update color sequence
  }
}

//Display stuff on the Neo7Segment displays
void colorChangingSequences(String timeStringOutput, String tempStringOutput)
{
  //Wait until the display is initialised before we try to show anything
  if (!timeDisplay.IsReady() || !tempDisplay.IsReady())
  {
    flashBoardLed(50, 1);
    delay(10);
    return;
  }
  
  timeDisplay.ForceUppercase(true);
  tempDisplay.ForceUppercase(true);

  uint32_t colorStart;
  uint32_t colorEnd;
  bool newShowUpdate = false;

  if (changeColor)
  {
    timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(red, green, blue));
    tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Color(red, green, blue));
    nextRainbow = millis() + 100;
    return;
  }

  if (!stateOn && !initDisplays)
  {
    timeDisplay.SetBrightness(0);
    tempDisplay.SetBrightness(0);
    timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 0, 0));
    tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Color(0, 0, 0));
    
    nextRainbow = millis() + 1000;
    return;
  }
  
  switch(displayFeature)
  {
    case 0: //Standard Colors
      uint32_t tempColor[tempDigits];
      for (int i=0; i<tempDigits/2; i++)
        tempColor[i] = waterStandardColor;
      for (int i=tempDigits/2; i<tempDigits; i++)
        tempColor[i] = tempStandardColor;
      timeDisplay.DisplayTextColor(timeStringOutput, timeStandardColor);
      tempDisplay.DisplayTextDigitColor(tempStringOutput, tempColor);
      nextRainbow = millis() + 100;
      break;
      
    case 1: //Rainbow Cycle
      timeDisplay.DisplayTextColorCycle(timeStringOutput, rainbowIndex);
      tempDisplay.DisplayTextColorCycle(tempStringOutput, rainbowIndex);
      nextRainbow = millis() + 10;
      rainbowIndex++;
      break;

    case 2: //Text color
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255));
      tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Wheel(rainbowIndex & 255));
      //timeDisplay.DisplayTextMarquee(timeStringOutput, 2, timeDisplay.Wheel(rainbowIndex & 255));
      //tempDisplay.DisplayTextMarquee(tempStringOutput, 2, tempDisplay.Wheel(rainbowIndex & 255));
      nextRainbow = millis() + 250;
      rainbowIndex+=5;
      break;

    case 3: //SPOON
      loopIndex++;
      if (loopIndex > 1)
        loopIndex = 0;

      timeDisplay.DisplayTextMarquee(timeStringOutput, loopIndex, timeDisplay.Wheel(rainbowIndex & 255));
      tempDisplay.DisplayTextMarquee(tempStringOutput, loopIndex, tempDisplay.Wheel(rainbowIndex & 255));
      nextRainbow = millis() + 250;
      rainbowIndex+=5;
      break;

    case 4: //Vertical Rainbow Cycle
      timeDisplay.DisplayTextVerticalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex+50) & 255));
      tempDisplay.DisplayTextVerticalRainbow(tempStringOutput, tempDisplay.Wheel(rainbowIndex & 255), tempDisplay.Wheel((rainbowIndex+50) & 255));
      nextRainbow = millis() + 50;
      rainbowIndex--;
      break;
      
    case 5: //Text Chaser
      loopIndex++;
      if (loopIndex > timePixels*tempPixels)
        loopIndex = 0;

      timeDisplay.DisplayTextChaser(timeStringOutput, loopIndex%timePixels, timeDisplay.Wheel(rainbowIndex & 255));
      tempDisplay.DisplayTextChaser(tempStringOutput, loopIndex%tempPixels, tempDisplay.Wheel(rainbowIndex & 255));
      nextRainbow = millis() + 50;
      rainbowIndex+=5;
      break;  

    case 6: //Horizontal Rainbow Cycle
      timeDisplay.DisplayTextHorizontalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex+50) & 255));
      tempDisplay.DisplayTextHorizontalRainbow(tempStringOutput, tempDisplay.Wheel(rainbowIndex & 255), tempDisplay.Wheel((rainbowIndex+50) & 255));
      nextRainbow = millis() + 50;
      rainbowIndex--;
      break;

    case 96: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(255, 0, 0));
      tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Color(255, 0, 0));
      nextRainbow = millis() + 1000;
      displayFeature++;
      break;

    case 97: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 255, 0));
      tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Color(0, 255, 0));
      nextRainbow = millis() + 1000;
      displayFeature++;
      break;

    case 98: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 0, 255));
      tempDisplay.DisplayTextColor(tempStringOutput, tempDisplay.Color(0, 0, 255));
      nextRainbow = millis() + 1000;
      displayFeature++;
      rainbowIndex = 0;
      break;

    case 99: //Initialize, show all colors in rainbow pattern
      timeDisplay.DisplayTextHorizontalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex + 50) & 255));
      tempDisplay.DisplayTextHorizontalRainbow(tempStringOutput, tempDisplay.Wheel(rainbowIndex & 255), tempDisplay.Wheel((rainbowIndex + 50) & 255));
      nextRainbow = millis() + 1;
      rainbowIndex ++;
      if (rainbowIndex >= 255)
      {
        rainbowIndex = 0;
        displayFeature = 0;
        rollingTimeTextIndex = 0;
        rollingTempTextIndex = 0;
        initDisplays = false;

        timeDisplay.DisplayTextColor("", timeDisplay.Color(0, 0, 0));
        tempDisplay.DisplayTextColor("", tempDisplay.Color(0, 0, 0));

        Sprintln("Initialization completed...");
      }
      break;
      
    default: //When pressing the button, if getting to last sequence, go into auto change mode until button is repressed
      displayFeature = 0;
      break;
  }
}
