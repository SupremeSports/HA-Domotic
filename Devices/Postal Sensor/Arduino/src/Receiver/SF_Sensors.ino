// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  //INPUTS
  pinMode(powerOledPin, INPUT_PULLUP);

  //OUTPUTS
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
