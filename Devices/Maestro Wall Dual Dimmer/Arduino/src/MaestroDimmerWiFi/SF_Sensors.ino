// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  //None
  
  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
  setBoardLED(LOW);

  pinMode(pinStandalone, INPUT_PULLUP);

  pinMode(selfResetPin, INPUT);
}

//INPUTS
void readSensors(bool all)
{
  //TODO
  
  if (!all)
    return;

  readVoltages();
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ANALOG SENSOR -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readVoltages()
{
  uint16_t volt5V = readAn(voltage5V_pin) * 5;

  voltage5VDimmer = volt5V/4095.0F*voltage5VRatio;
  
  Sprint("Local 5V: ");
  Sprint(voltage5VDimmer);
  Sprintln("V");
  Sprint("Remote 5V: ");
  Sprint(voltage5VSwitch);
  Sprintln("V");
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(1);
  return(analogRead(pinToRead));
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- Utility functions -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void setBoardLED(bool newState)
{
  if (!enableBoardLED)
    newState = LOW;

  digitalWrite(boardLedPin, boardLedPinRevert ? !newState : newState);
}

void flashBoardLed(int delayFlash, int qtyFlash)
{
  for (int i=0; i<qtyFlash; i++)
  {
    setBoardLED(HIGH);
    local_delay(delayFlash);
    setBoardLED(LOW);
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

  readSensors(true);
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
