// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- FAN DIMMER ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void runFanButtons()
{
  checkFanUP();
  checkFanON();
  checkFanDN();
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- BUTTONS ----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkFanON()
{
  btnFanON.update();

  //Single/Long click sets the lamp on/off and returns to preset value
  if (btnFanON.isSingleClick() || btnFanON.isLongClick()) //Single or long is not distinguish for this button
  {
    Sprintln("Fan ON single click");

    Sprintln(fan.state ? "Fan Off" :"Fan On");
    fan.state = !fan.state;
    fan.full = false;
    sendDataNRF = true;
    buttonPressed = true;

    if (!fan.state)
      lastMinute = millis()-60000; //Force update when going OFF
  }

  //Double click sets the full brightness mode or returns to preset value
  if (btnFanON.isDoubleClick())
  {
    Sprintln("Fan ON double click");
    fan.state = true;
    fan.full = !fan.full;
    sendDataNRF = true;
    buttonPressed = true;
  }
}

void checkFanUP()
{
  btnFanUP.update();

  if (btnFanUP.isClick())
  {
    Sprintln("Fan UP click");

    if (fan.state)
      fan.level++; //Only change value if already ON
    fan.level = constrain(fan.level, fanMin, fanMax);
    fan.state = true;
    fan.full = false;
    sendDataNRF = true;
    buttonPressed = true;
    
    Sprintln(fan.level);
  }
}

void checkFanDN()
{
  btnFanDN.update();
  
  if (btnFanDN.isClick())
  {
    Sprintln("Fan DN click");

    if (fan.state)
      fan.level--; //Only change value if already ON
    fan.level = constrain(fan.level, fanMin, fanMax);
    fan.state = true;
    fan.full = false;
    sendDataNRF = true;
    buttonPressed = true;
    
    Sprintln(fan.level);
  }
}
