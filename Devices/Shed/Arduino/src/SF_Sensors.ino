// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  #ifdef EXTERNAL_EN
    dht.begin();
  #endif
  #ifdef INTERNAL_EN  
    dhtInternal.begin();
  #endif
  
  //INPUTS
  //pinMode(selfReset_pin, INPUT_PULLUP);

  pinMode(mainDoorClosedPin, INPUT_PULLUP);
  pinMode(leftDoorsClosedPin, INPUT_PULLUP);
  pinMode(rightDoorsClosedPin, INPUT_PULLUP);

  pinMode(mainDoorLockPin, INPUT_PULLUP);

  //pinMode(mainIntLightSwPin, INPUT_PULLUP);   //declared in initSwitches()
  
  //OUTPUTS
  for (int i=0; i<relayQty; i++)
  {
    if (relayPin[i] != 0)
      pinMode(relayPin[i], OUTPUT);
  }

  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

//INPUTS
void readSensors(bool all)
{
  readDoorsStatus();

  readLightToggles(lightIntSwPtr); //Toggling light for every state change

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
  //Todo
  writeRelays();
}

void writeRelays()
{
  for (int i=0; i<relayQty; i++)
  {
    if (relayPin[i] != 0)
    {
      if (relayHld[i] != 0)
        checkHold(i);
      digitalWrite(relayPin[i], relayCmd[i]);
    }

    local_delay(1);
  }
}

void checkHold(uint8_t ptr)
{
  if (relayCmd[ptr] && !relayOns[ptr])
  {
    relayOns[ptr] = true;
    relayTmr[ptr] = millis();
  }

  if (!relayCmd[ptr] && relayOns[ptr])
  {
    relayOns[ptr] = false;
    forceMqttUpdate = true;
  }

  if (relayCmd[ptr] && (millis()-relayTmr[ptr])>(relayHld[ptr]*1000))
  {
    relayCmd[ptr] = false;
    forceMqttUpdate = true;
  }
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ANALOG SENSOR -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  //Todo
  local_delay(1);
  return;
}

void readVoltages()
{
  uint16_t volt5V = readAn(voltage5V_pin) * 5;
  local_delay(10);
  uint16_t volt12V = readAn(voltage12V_pin) * 5;

  voltage5V = volt5V/1023.0F*voltage5VRatio;
  voltage12V = volt12V/1023.0F*voltage12VRatio;

  Sprint("12V: ");
  Sprint(voltage12V);
  Sprintln("V");
  Sprint("5V: ");
  Sprint(voltage5V);
  Sprintln("V");

  local_delay(1);
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(10);
  return(analogRead(pinToRead));
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readDoorsStatus()
{
  mainDoorStatus = readDoorPin(mainDoorClosedPin, mainDoorStatus);
  leftDoorsStatus = readDoorPin(leftDoorsClosedPin, leftDoorsStatus);
  rightDoorsStatus = readDoorPin(rightDoorsClosedPin, rightDoorsStatus);

  mainDoorLockStatus = readDoorPin(mainDoorLockPin, mainDoorLockStatus);
}

//Door pin detection and instant send to MQTT if state has changed
bool readDoorPin(uint8_t pin, bool Status)
{
  bool doorClosed = !digitalRead(pin);

  if (Status != doorClosed)
  {
    forceMqttUpdate = true;
    return doorClosed;
  }

  return Status;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- DHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifdef EXTERNAL_EN
void readDHT()
{
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  //Output values
  DHTTempOut = isnan(temp) ? initValue : temp;
  DHTHumOut = isnan(hum) ? initValue : hum;

  Sprint("TempOut: ");
  Sprint(DHTTempOut);
  Sprintln("*C");
  Sprint("HumOut: ");
  Sprint(DHTHumOut);
  Sprintln("%");
  
  local_delay(1);
} 
#endif

#ifdef INTERNAL_EN
void readInternalDHT()
{
  float temp = dhtInternal.readTemperature();
  float hum = dhtInternal.readHumidity();

  Sprintln(temp);
  Sprintln(hum);

  //Output values
  DHTTempIn = isnan(temp) ? initValue : temp;
  DHTHumIn = isnan(hum) ? initValue : hum;

  Sprint("TempIn: ");
  Sprint(DHTTempIn);
  Sprintln("*C");
  Sprint("HumIn: ");
  Sprint(DHTHumIn);
  Sprintln("%");

  local_delay(1);
}
#endif

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- RESET PIN ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void forceReset()
{
  Sprintln("Forced Reset...");
  delay(500);
  resetBoard();
}

void resetBoard()
{
  pinMode(selfReset_pin, OUTPUT);
  digitalWrite(selfReset_pin, LOW);

  local_delay(10000);
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
