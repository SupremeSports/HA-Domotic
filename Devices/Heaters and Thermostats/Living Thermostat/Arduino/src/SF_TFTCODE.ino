// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- TFT CODE ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
/********************************************************************//**
 * @brief     draws a result box after code confirmation with ok button
 * @param[in] color background color of result box
 * @param[in] test  string to display in result box area
 * @param[in] xPos  X position of text output
 * @return    None
 *********************************************************************/
 void drawCodeScreen()
{
  if (PMode != PM_CODE)
    return;
    
  //clear screen black
  tft.fillRect(0, 0, 240, 320, ILI9341_BLACK);
  
  tft.setTextSize(0);
  tft.setFont(&FreeSansBold24pt7b);
  
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, ILI9341_CYAN);

  //Draw C and OK field   
  tft.fillRect(0, 260, 80, 60, ILI9341_RED);
  tft.fillRect(160, 260, 80, 60, ILI9341_GREEN);
  
  //Draw Horizontal Lines
  for (int h=80; h<=320; h+=60)
  tft.drawFastHLine(0, h, 240, ILI9341_WHITE);

  //Draw Vertical Lines
  for (int v=80; v<=240; v+=80)
  tft.drawFastVLine(v, 80, 240, ILI9341_WHITE);

  //Display keypad labels 
  for (int j=0; j<4; j++)
  {
    for (int i=0; i<3; i++)
    {
      tft.setCursor(28 + (80*i), 125 + (60*j));
      if ((j==3) && (i==2))
        tft.setCursor(5 + (80*i), 125 + (60*j));  //Readjust position for OK button
      if ((j==3) && (i==0))
        tft.setCursor(23 + (80*i), 125 + (60*j)); //Readjust position for C button
      
      tft.setFont(&FreeSansBold24pt7b);
      tft.setTextColor(ILI9341_WHITE);
      tft.println(symbol[j][i]);
    }
  }
}

/********************************************************************//**
 * @brief     shows the entered numbers (stars)
 * @param[in] None
 * @return    None
 *********************************************************************/
void displayResult()
{
  if (PMode != PM_CODE)
    return;

  PrevMode = PMode;
    
  String s1="";
  tft.fillRect(0, 0, 240, 80, ILI9341_CYAN);  //clear result box
  
  tft.setCursor(10, 100);
  tft.setTextSize(2);
  tft.setFont(&FreeSansBold24pt7b);
    
  tft.setTextColor(ILI9341_BLACK);
  if (Number == 0)
    tft.println(" ");
  else
  { 
    for (int i=0; i<String(Number).length(); i++)
      s1 = s1 + "*";
    tft.println(s1); //update new value
  }   
}

/********************************************************************//**
 * @brief     draws a result box after code confirmation with ok button
 * @param[in] color background color of result box
 * @param[in] test  string to display in result box area
 * @param[in] xPos  X position of text output
 * @return    None
 *********************************************************************/
void drawResultBox(int color, char text[10], int xPos)
{
  if (PMode != PM_CODE)
    return;
    
  //Draw the Result Box
  tft.fillRect(0, 0, 240, 80, color);
 
  tft.setCursor(xPos, 60);
  tft.setTextSize(0);
  tft.setFont(&FreeSansBold24pt7b);
  
  // draw text
  tft.println(text);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TOUCH CONTROL -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void detectCodeButtons()
{
  // CODE screen
  bool detectOK = false;
  
  if (X>0 && X<75) //Detecting Buttons on Column 1
  {
    if (Y>265) //If clear Button is pressed
    {
      Sprintln("Button Clear");
      Number = 0;
    }
    
    if (Y>205 && Y<255) //If Button 1 is pressed
    {
      Sprintln("Button 1");
      buttonAckTone();
      Number = Number==0 ? 1 : (Number*10) + 1;//Pressed twice
    }
    
    if (Y>145 && Y<195) //If Button 4 is pressed
    {
      Sprintln("Button 4");
      buttonAckTone();
      Number = Number==0 ? 4 : (Number*10) + 4;//Pressed twice
    }
    
    if (Y>85 && Y<135) //If Button 7 is pressed
    {
      Sprintln("Button 7");
      buttonAckTone();
      Number = Number==0 ? 7 : (Number*10) + 7;//Pressed twice
    } 
  }

  if (X>85 && X<175) //Detecting Buttons on Column 2
  {
    if (Y>265)
    {
      Sprintln("Button 0"); //Button 0 is Pressed
      buttonAckTone();
      Number = Number==0 ? 0 : (Number*10) + 0;//Pressed twice
    }
    
    if (Y>205 && Y<255)
    {
      Sprintln("Button 2");
      buttonAckTone();
      Number = Number==0 ? 2 : (Number*10) + 2;//Pressed twice
    }
    
     if (Y>145 && Y<195)
    {
      Sprintln("Button 5");
      buttonAckTone();
      Number = Number==0 ? 5 : (Number*10) + 5;//Pressed twice
    }
    
    if (Y>85 && Y<135)
    {
      Sprintln("Button 8");
      buttonAckTone();
      Number = Number==0 ? 8 : (Number*10) + 8;//Pressed twice
    }   
  }

  if (X>185) //Detecting Buttons on Column 3
  {
    if (Y>265)
    {
      Sprintln("Button OK");
      detectOK = true;
    }
    
    if (Y>205 && Y<255)
    {
      Sprintln("Button 3");
      buttonAckTone();
      Number = Number==0 ? 3 : (Number*10) + 3;//Pressed twice
    }
    
    if (Y>145 && Y<195)
    {
      Sprintln("Button 6");
      buttonAckTone();
      Number = Number==0 ? 6 : (Number*10) + 6;//Pressed twice
    }
    
    if (Y>85 && Y<135)
    {
      Sprintln("Button 9");
      buttonAckTone();
      Number = Number==0 ? 9 : (Number*10) + 9;//Pressed twice
    }
  }

  if (detectOK)
  {
    Sprint("Code: ");
    Sprintln(Number);
    
    if (Number == passcode)
    {
      drawResultBox(ILI9341_GREEN,"CODE OK",10);
      
      #ifdef ENABLE_BEEPER
        tone(BEEPER,1000,800);
      #endif
      
      //PMode = PM_OPTION;    //Enable this to go to OPTIONS menu once logged in
      PMode = PM_MAIN;        //Enable this to go to MAIN menu once logged in
      logged = true;
      logMillis = millis() + logDelayOff*1000;
    }
    else
    {
      drawResultBox(ILI9341_RED, "WRONG",30);

      #ifdef ENABLE_BEEPER
        for (int i=0;i< 3;i++)
        {
          tone(BEEPER,4000);
          local_delay(100);
          noTone(BEEPER);
          local_delay(50);
        }
      #endif
      
      PMode = PM_MAIN;        //Return to main as passcode is incorrect
      logged = false;         //Stay logged out
    }
    
    local_delay(1000);
    Number = 0;
    
    return;
  }

  displayResult();
}
