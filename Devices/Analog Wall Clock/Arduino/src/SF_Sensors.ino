// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
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
void readSensors(bool all)
{
  readHomingButton();

  if (!all)
    return;
    
  readAnalogSensors();
  readVoltages();

  #ifdef EXTERNAL_EN
    readDHT();
  #endif
  #ifdef INTERNAL_EN
    readInternalDHT();
  #endif
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
  local_delay(1);
  return;
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(10);
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

void readVoltages()
{
  #ifdef ESP32
    float volt5V = ESP.getVcc();
    voltage5V = volt5V/4095.0F*voltage5VRatio;
  #elif ESP8266
    float volt5V = ESP.getVcc();
    voltage5V = volt5V/1023.0F*voltage5VRatio;
  #endif

  #ifdef DEBUG
    Sprint("3V3: ");
    Sprint(voltage5V);
    Sprintln("V");
  #endif
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readHomingButton()
{
  if (clockHomed && !digitalRead(ResetHoming))
  {
    clockHomed = false;
    initialHourOns = false;
    initialMinOns = false;
    initialSecOns = false;
  }
}

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
