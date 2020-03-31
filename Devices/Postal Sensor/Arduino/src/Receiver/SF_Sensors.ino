// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  //INPUTS
  pinMode(powerOledPin, INPUT_PULLUP);

  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
  
  pinMode(resetOledPin, OUTPUT);
}

//INPUTS
void readSensors()
{
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
  
  if (enableBoardLED)
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

  //readSensors();
  sendSensors();
}
