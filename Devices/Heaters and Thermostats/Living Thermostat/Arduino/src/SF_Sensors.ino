// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  #ifdef ENABLE_DHT
    setRxTxGpio(false, true);
    local_delay(100);
    dht.begin();
    local_delay(100);
    readDHT();
  #endif

  //INPUTS
  //TODO

  //OUTPUTS
  pinMode(TFT_LED, OUTPUT);
  writeOutputs();
  
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output

  Sprintln("Sensors Initiated!");
}

//INPUTS
void readSensors(bool all)
{
  #ifndef ENABLE_DHT  //If not in use, simulate data more often
    readDHT();
  #endif
  
  if (!all)
    return;

  lastKeepAlive += 5; //Add 5 seconds to counter
  hvacAlive = lastKeepAlive<keepAlive;

  readAnalogSensors();
  readDigitalSensors();
  readVoltages();

  #ifdef ENABLE_DHT
    readDHT();
  #endif
}

//OUTPUTS
void writeOutputs()
{
  //digitalWrite(TFT_LED, screenOff);   // LOW to turn backlight on - pcb version 01-02-00
  digitalWrite(TFT_LED, !screenOff);    // HIGH to turn backlight on - pcb version 01-01-00
  return;
}

//CHANGE PIN FUNCTION  TO GPIO
// If boolean is true, it will be set as GPIO
void setRxTxGpio(bool Tx, bool Rx)
{
  pinMode(1, Tx ? FUNCTION_3 : FUNCTION_0);
  pinMode(3, Rx ? FUNCTION_3 : FUNCTION_0);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ANALOG SENSOR -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  return;
}

void readVoltages()
{
  return;
  uint16_t volt5V = readAn(voltage5V_pin) * 5;

  voltage5V = volt5V/1023.0F*voltage5VRatio;
  
  Sprint("5V: ");
  Sprint(voltage5V);
  Sprintln("V");
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(1);
  return(analogRead(pinToRead));
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readDigitalSensors()
{
  return;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- DHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readDHT()
{
  #ifdef ENABLE_DHT
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    //Output values
    iRoom_temperature = isnan(temp) ? initValue : temp;
    iRoom_humidity = isnan(hum) ? initValue : hum;

    Sprint("TempOut: ");
    Sprint(iRoom_temperature);
    Sprintln("*C");
    Sprint("HumOut: ");
    Sprint(iRoom_humidity);
    Sprintln("%");
    
    local_delay(1);
  #else
    iRoom_temperature += 0.1;
    if (iRoom_temperature > MAX_TEMPERATURE)
      iRoom_temperature = MIN_TEMPERATURE;
  
    iRoom_humidity += 0.1;
    if (iRoom_humidity > 149.9)
      iRoom_humidity = -100;
  #endif
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
  for (int i=0; i<qtyFlash; i++)
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
