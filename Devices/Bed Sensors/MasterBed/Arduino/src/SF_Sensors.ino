// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  initEEPROM();
  readEEPROM();
  setScale();
  powerUpScale();
  
  //INPUTS
  //None
  
  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

//INPUTS
void readSensors()
{
  readScale();
  
  return;
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

void setScale()
{
  powerUpScale();
  readEEPROM();
  local_delay(50);
  scale.set_scale(loadcellCalib<1 ? 1 : loadcellCalib);
}

void setTare()
{
  powerUpScale();
  local_delay(50);
  scale.tare(); // Reset the scale to 0
}

void readScale()
{
  if (!triggerScale || !scale.is_ready())//|| (millis()-scaleMillis < 1000) 
    return;
    
  outputScale = scale.get_units(5) * -1.0;

  scale.power_down(); //Send sensor back to sleep

  triggerScale = false;
}

void powerUpScale()
{
  scale.power_up();

  //scaleMillis = millis(); //Prestart sensor 1 second before reading it
  triggerScale = true;
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
