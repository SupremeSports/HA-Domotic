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
  #ifdef ESP32
    ledcSetup(TFT_LED_CH, TFT_LED_FREQ, TFT_LED_RES);
    ledcAttachPin(TFT_LED, TFT_LED_CH);
    #ifdef TFT_LED_HIGH
      ledcWrite(TFT_LED_CH, 0);
    #else
      ledcWrite(TFT_LED_CH, 255); //Invert mapping if active LOW
    #endif
  #elif ESP8266
    pinMode(TFT_LED, OUTPUT);
  #endif
  writeOutputs();

  //Beeper configuration
  Sprintln("Init Beeper...");
  #ifdef ENABLE_BEEPER
    #ifdef ESP32
      ledcSetup(BEEPER_CH, BEEPER_FREQ, BEEPER_RES);
      ledcAttachPin(BEEPER_PIN, BEEPER_CH);
    #elif ESP8266
      noTone(BEEPER_PIN);
    #endif
  #else
    Sprintln("Beeper Disabled!");
  #endif
  
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
  setScreenBrightness();
}

//CHANGE PIN FUNCTION  TO GPIO
// If boolean is true, it will be set as GPIO
void setRxTxGpio(bool Tx, bool Rx)
{
  #ifdef ESP8266
    pinMode(1, Tx ? FUNCTION_3 : FUNCTION_0);
    pinMode(3, Rx ? FUNCTION_3 : FUNCTION_0);
  #endif
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TFT BRIGHTNESS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setScreenBrightness()
{
  #ifdef ESP32
    int screenValue = 254;  //Full ON
    if (screenOff)
      screenValue = 0;      //Full OFF
    else if (screenDim)
    {
      
    #ifdef TFT_LED_HIGH
      screenValue = map(screenDimValue, 0, 7, 30, 255);
    #else
      screenValue = map(screenDimValue, 0, 7, 255, 30); //Invert mapping if active LOW
    #endif
    }
  
    ledcWrite(TFT_LED_CH, screenValue);
  #elif ESP8266
    #ifdef TFT_LED_HIGH
      digitalWrite(TFT_LED, !screenOff);     // HIGH to turn backlight on - pcb version 01-01-00
    #else
      digitalWrite(TFT_LED, screenOff);      // LOW to turn backlight on - pcb version 01-02-00
    #endif
  #endif
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- ANALOG SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  //touch.getVBat()
  //touch.getAuxIn()
  //touch.getTemp()
  //touch.getTempF()
  return;
}

void readVoltages()
{
  #ifdef ENABLE_VOLT_ADC
    float volt5V = readAn(voltage5V_pin) * 5.5;

    #ifdef ESP32
      voltage5V = volt5V/4095.0F*voltage5VRatio;
    #elif ESP8266
      voltage5V = volt5V/1023.0F*voltage5VRatio;
    #endif
  #else
    voltage5V = touch.getVBat();
  #endif

  /*Sprint("5V: ");
  Sprint(voltage5V);
  Sprintln("V");*/
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
  #else //Simulate values
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
