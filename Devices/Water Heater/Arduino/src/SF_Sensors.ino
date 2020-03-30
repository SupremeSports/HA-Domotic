// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  //INPUTS
  pinMode(waterFlowPin, INPUT);
  
  //OUTPUTS
  pinMode(waterHeaterPin, OUTPUT);
  pinMode(tempInPin, OUTPUT);
  pinMode(tempOutPin, OUTPUT);
  writeOutputs();                                    //Make sure output is set ON/OFF as soon as possible
  
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

//INPUTS
void readSensors()
{
  readWaterTemp();
  return;
}

//OUTPUTS
void writeOutputs()
{
  digitalWrite(waterHeaterPin, waterHeaterOn);
  return;
}

void readWaterTemp()
{
  int temp = 0;
  
  //Read input temperature
  digitalWrite(tempInPin, HIGH);
  local_delay(10);
  temp = analogRead(tempPin);
  local_delay(10);
  digitalWrite(tempInPin, LOW);
  inputWaterTemp = inTemp_ax2*pow(float(temp), 2) + inTemp_bx*temp + inTemp_c;

  //Read output temperature
  digitalWrite(tempOutPin, HIGH);
  local_delay(10);
  temp = analogRead(tempPin);
  local_delay(10);
  digitalWrite(tempOutPin, LOW);
  outputWaterTemp = outTemp_ax2*pow(float(temp), 2) + outTemp_bx*temp + outTemp_c;
}

void readWaterFlow()
{
  if (millis()-waterFlowSecond >= 1000)
  {
    inputWaterFlow = waterFlowBuffer;
    
    //If water flow is greater than 5L/min, read temperature once per second
    if (inputWaterFlow > 2)
      readWaterTemp();

    waterFlowBuffer = 0;
    waterFlowSecond = millis();
  }
  
  byte waterFlowOn = digitalRead(waterFlowPin) ? 1 : 0;
  if (waterFlowOn != waterFlowPrev)
  {
    waterFlowBuffer++;
    waterFlowPrev = waterFlowOn;
  }
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
