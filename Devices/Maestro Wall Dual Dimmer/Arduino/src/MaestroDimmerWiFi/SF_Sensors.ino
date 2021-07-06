// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  //None
  
  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);
  
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

  readAnalogSensors();
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
void readAnalogSensors()
{
  //TODO
  local_delay(1);
  return;
}

void readVoltages()
{
  uint16_t volt5V = readAn(voltage5V_pin) * 5;

  voltage5VDimmer = volt5V/4095.0F*voltage5VRatio;

  #ifdef DEBUG
    Sprint("Local 5V: ");
    Sprint(voltage5VDimmer);
    Sprintln("V");
    Sprint("Remote 5V: ");
    Sprint(voltage5VSwitch);
    Sprintln("V");
  #endif
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(1);
  return(analogRead(pinToRead));
}

float readAnAvg(uint8_t pinToRead, uint16_t iterations)
{
  uint16_t samples[iterations];
  analogRead(pinToRead); //Dump first reading
  for (int i=0; i<iterations; i++)
  {
    samples[i] = analogRead(pinToRead);
    delay(10);
  }

  float average = 0;
  for (int i=0; i<iterations; i++)
     average += samples[i];

  average /= iterations;
  
  return average; //Return average of values
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//TODO

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- Utility functions -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void setBoardLED(bool newState)
{
  if (enableBoardLED)
    digitalWrite(boardLedPin, boardLedPinRevert ? !newState : newState);
  else
    digitalWrite(boardLedPin, boardLedPinRevert);
}

void flashBoardLed(int delayFlash, int qtyFlash)
{
  for (int i=0; i < qtyFlash; i++)
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
  if (networkActive)
    flashBoardLed(2, 1);
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
