// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  //INPUTS

  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                  // Initialize the LED_BUILTIN pin as an output

  pinMode(speakerOut, OUTPUT);
  digitalWrite(speakerOut, LOW);

  pinMode(speakerEN, OUTPUT);
  digitalWrite(speakerEN, LOW);
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
    delay(delayFlash);
    setBoardLED(boardLedPinRevert);
    delay(delayFlash);
  }
}

//Short flash every 5 seconds when everything is ok
void flashEvery5sec()
{
  if (millis()-ledFlashDelay < 5000)
    return;

  flashBoardLed(2, 1);
    
  ledFlashDelay = millis();
}
