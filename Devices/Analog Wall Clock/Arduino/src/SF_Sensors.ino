void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  pinMode(HourHall, INPUT);
  pinMode(MinHall, INPUT);
  pinMode(SecHall, INPUT);

  pinMode(ResetHoming, INPUT);

  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output

  pinMode(StepEnable, OUTPUT);
  digitalWrite(StepEnable, HIGH);
  
  pinMode(HourPulse, OUTPUT);
  pinMode(MinPulse, OUTPUT);
  pinMode(SecPulse, OUTPUT);

}

//INPUTS
void readSensors()
{
  if (clockHomed && !digitalRead(ResetHoming))
  {
    clockHomed = false;
    initialHourOns = false;
    initialMinOns = false;
    initialSecOns = false;
  }
  
  return;
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- Utility functions -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void setBoardLED(bool newState)
{
  if (!enableBoardLED)
    newState = boardLedPinRevert;
  
  digitalWrite(boardLedPin, newState);
}

void flashBoardLed(int delayFlash, int qtyFlash)
{
  for (int i=0; i < qtyFlash; i++)
  {
    setBoardLED(!boardLedPinRevert);
    local_delay(delayFlash);
    setBoardLED(boardLedPinRevert);
    local_delay(delayFlash);
  }
}

//Short flash every 5 seconds when everything is ok
void flashEvery5sec()
{
  if (millis()-ledFlashDelay < 5000)
    return;

  if (networkActive)
    flashBoardLed(2, 1);
    
  ledFlashDelay = millis();
  
  sendSensors();
}
