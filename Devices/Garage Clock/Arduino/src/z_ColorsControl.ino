// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- COLORS FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void changeColorButton()
{
  if (!changeColor)
    return;
    
  if (colorAutoSwitch)
    displayFeature = 0;   //If running in auto change mode, return to standard colors when pressing on button
  else
    displayFeature += 1;  //Otherwise, increase feature ID

  nextRainbow = millis()-1;
  nextSwitch = millis()-1;
  colorAutoSwitch = false;
  changeColor = false;
  loopIndex = 0;
  rainbowIndex = 0;

  #ifdef DEBUG
    Serial.print("Display feature: ");
    Serial.println(displayFeature);
  #endif
}

void autoColorChanging()
{
  changeColorButton();  //Check if a sequence change has been requested
  
  //Switch what we are showing every 5 seconds
  if (millis() > nextSwitch && colorAutoSwitch)
  {
    nextSwitch = millis() + 5000;
    displayFeature = (displayFeature + 1) % 7; //Modulo number of cases (0 to 6 = 7)
    nextRainbow = millis()-1;

    #ifdef DEBUG
      Serial.print("Display feature: ");
      Serial.println(displayFeature);
    #endif
  }

  if (nextRainbow < millis())
    colorChangingSequences(); //Update color sequence
}

//Display stuff on the Neo7Segment displays
void colorChangingSequences()
{
  bool newShowUpdate = false;
  
  switch(displayFeature)
  {
    case 0: //Rainbow Cycle Fast
      rainbow(rainbowIndex);
      nextRainbow = millis() + 5;
      rainbowIndex++;
      break;

    case 1: //Rainbow Cycle Slow
      rainbow(rainbowIndex);
      nextRainbow = millis() + 100;
      rainbowIndex++;
      break;

    case 2: //Standard Color Red
      colorFill(strip.Color(255, 0, 0));
      nextRainbow = millis() + 60000;
      break;

    case 3: //Standard Color Green
      colorFill(strip.Color(0, 255, 0));
      nextRainbow = millis() + 60000;
      break;

    case 4: //Standard Color Blue
      colorFill(strip.Color(0, 0, 255));
      nextRainbow = millis() + 60000;
      break;

    case 5: //Standard Color White
      colorFill(strip.Color(127, 127, 127));
      nextRainbow = millis() + 60000;
      break;
      
    case 6: //Standard Colors Cycle RGB
      if (rainbowIndex % 10 == 0)
      {
        loopIndex++;
        newShowUpdate = true;
      }
      else
      {
        nextRainbow = millis() + 100;
        rainbowIndex++;
        break;
      }

      if (loopIndex > 2)
        loopIndex = 0;

      if (newShowUpdate)
      {
        if (loopIndex == 0)
          colorFill(strip.Color(255, 0, 0));
        else if (loopIndex == 1)
          colorFill(strip.Color(0, 255, 0));
        else if (loopIndex == 2)
          colorFill(strip.Color(0, 0, 255));
          
        newShowUpdate = false;
      }
        
      nextRainbow = millis() + 100;
      rainbowIndex++;
      break;

    case 96: //Initialize, show all colors one by one
      colorWipe(strip.Color(255, 0, 0), rainbowIndex);        // Red
      nextRainbow = millis();
      rainbowIndex++;
      if (rainbowIndex >= strip.numPixels())
      {
        displayFeature++;
        rainbowIndex = 0;
        nextRainbow = millis() + 1000;
      }
      break;

    case 97: //Initialize, show all colors one by one
      colorWipe(strip.Color(0, 255, 0), rainbowIndex);        // Green
      nextRainbow = millis();
      rainbowIndex++;
      if (rainbowIndex >= strip.numPixels())
      {
        displayFeature++;
        rainbowIndex = 0;
        nextRainbow = millis() + 1000;
      }
      break;

    case 98: //Initialize, show all colors one by one
      colorWipe(strip.Color(0, 0, 255), rainbowIndex);        // Blue
      nextRainbow = millis();
      rainbowIndex++;
      if (rainbowIndex >= strip.numPixels())
      {
        displayFeature++;
        rainbowIndex = 0;
        nextRainbow = millis() + 1000;
      }
      break;

    case 99: //Initialize, show all colors in rainbow pattern
      rainbow(rainbowIndex);
      nextRainbow = millis();
      rainbowIndex++;
      if (rainbowIndex >= 255)
      {
        rainbowIndex = 0;
        displayFeature = 1;
        colorAutoSwitch = true;
        #ifdef DEBUG
          Serial.println("Initialization completed...");
        #endif
      }
      break;
      
    default: //When pressing the button, if getting to last sequence, go into auto change mode until button is repressed
      colorAutoSwitch = true;
      break;
  }
  Serial.println(displayFeature);
  Serial.println(rainbowIndex);
}
