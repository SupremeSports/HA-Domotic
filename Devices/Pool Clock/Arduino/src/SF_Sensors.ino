// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS

  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

//INPUTS
void readSensors()
{
  getLightLevel();

  return;
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

void getLightLevel()
{
  int level = map(initDisplays ? defaultBrightness : brightness, 0, 255, 0, 100);
  level = constrain(level, 0, 100);

  if (initDisplays)
    level = defaultBrightness;

  timeDisplay.SetBrightness(level);
  tempDisplay.SetBrightness(level);
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

float kelvinToFahrenheit(float kelvin)
{
  float celsius = kelvin - 273.15;
  return (celsiusToFahrenheit(celsius));
}

float kelvinToCelsius(float kelvin)
{
  return (kelvin - 273.15);
}

float celsiusToFahrenheit(float celsius)
{
  return (((9.0/5.0) * celsius) + 32);
}

float fahrenheitToCelsius(float fahrenheit)
{
  return ((5.0/9.0) * (fahrenheit - 32));
}
