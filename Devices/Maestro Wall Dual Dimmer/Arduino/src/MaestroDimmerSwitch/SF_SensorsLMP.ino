// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- LAMP DIMMER --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void runLmpButtons()
{
  checkLmpUP();
  checkLmpON();
  checkLmpDN();
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- BUTTONS ----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkLmpON()
{
  btnLmpON.update();

  //Single/Long click sets the lamp on/off and returns to preset value
  if (btnLmpON.isSingleClick() || btnLmpON.isLongClick()) //Single or long is not distinguish for this button
  {
    Sprintln("Lamp ON single click");

    Sprintln(lamp.state ? "Light Off" :"Light On");
    lamp.state = !lamp.state;
    lamp.full = false;
    sendDataNRF = true;
    buttonPressed = true;

    if (!lamp.state)
      lastMinute = millis()-60000; //Force update when going OFF
  }

  //Double click sets the full brightness mode or returns to preset value
  if (btnLmpON.isDoubleClick())
  {
    Sprintln("Lamp ON double click");
    lamp.state = true;
    lamp.full = !lamp.full;
    sendDataNRF = true;
    buttonPressed = true;
  }
}

void checkLmpUP()
{
  btnLmpUP.update();

  if (btnLmpUP.isClick())
  {
    Sprintln("Lamp UP click");

    if (lamp.state)
      lamp.level++; //Only change value if already ON
    lamp.level = constrain(lamp.level, lampMin, lampMax);
    lamp.state = true;
    lamp.full = false;
    sendDataNRF = true;
    buttonPressed = true;
    
    Sprintln(lamp.level);
  }
}

void checkLmpDN()
{
  btnLmpDN.update();
  
  if (btnLmpDN.isClick())
  {
    Sprintln("Lamp DN click");

    if (lamp.state)
      lamp.level--; //Only change value if already ON
    lamp.level = constrain(lamp.level, lampMin, lampMax);
    lamp.state = true;
    lamp.full = false;
    sendDataNRF = true;
    buttonPressed = true;
    
    Sprintln(lamp.level);
  }
}
