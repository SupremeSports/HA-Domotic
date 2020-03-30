// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  //TODO
  
  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

//INPUTS
void readSensors()
{
  outputAirTemp = readOutputAirTemp();
  return;
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

float readOutputAirTemp()
{
  int temp = analogRead(tempPin);
  
  return temp_ax2*pow(float(temp), 2) + temp_bx*temp + temp_c;
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

  readSensors();
  sendSensors();
}

//float kelvinToFahrenheit(float kelvin)
//{
//  float celsius = kelvin - 273.15;
//  return (celsiusToFahrenheit(celsius));
//}
//
//float kelvinToCelsius(float kelvin)
//{
//  return (kelvin - 273.15);
//}
//
//float celsiusToFahrenheit(float celsius)
//{
//  return (((9.0/5.0) * celsius) + 32);
//}
//
//float fahrenheitToCelsius(float fahrenheit)
//{
//  return ((5.0/9.0) * (fahrenheit - 32));
//}
