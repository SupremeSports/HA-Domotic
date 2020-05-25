// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  dht.begin();
  dhtInternal.begin();

  //INPUTS
  pinMode(poolLvlHi_pin, INPUT_PULLUP);
  pinMode(poolLvlLo_pin, INPUT_PULLUP);

  pinMode(poolCabinDoor_pin, INPUT_PULLUP);
  pinMode(poolCabinLocked_pin, INPUT_PULLUP);

  pinMode(selfReset_pin, INPUT_PULLUP);

  //OUTPUTS
  for (int i=0; i<relayQty; i++)
    pinMode(relayPin[i], OUTPUT);

  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output

  //SERVOS
  flowHeater_servo.attach(flowHeater_pin);
  flowSlide_servo.attach(flowSlide_pin);
  flowSkimDrain_servo.attach(flowSkimDrain_pin);
  writeServos();
}

//INPUTS
void readSensors(bool all)
{
  readLevelSensors();
  readDoorSensors();

  if (!all)
    return;

  readAnalogSensors();
  readVoltages();

  readDHT();
  readInternalDHT();
}

//OUTPUTS
void writeOutputs()
{
  writeRelays();
  writeServos();
}

void writeRelays()
{
  for (int i=0; i<relayQty; i++)
    digitalWrite(relayPin[i], relayCmd[i]);
}

void writeServos()
{
  flowSkimDrain_servo.write(map(flowSkimDrain, 0, 100, 18, 180));
  flowSlide_servo.write(map(flowSlide, 0, 100, 105, 0));
  flowHeater_servo.write(map(flowHeater, 0, 100, 100, 0));
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ANALOG SENSOR -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  poolPH = readAn(poolPH_pin);
  poolPress = readAn(poolPress_pin);
  poolTempIn = readAn(poolTempIn_pin);
  poolTempOut = readAn(poolTempOut_pin);
  poolPumpTemp = readAn(poolPumpTemp_pin);
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
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  return(analogRead(pinToRead));
}
// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readLevelSensors()
{
  poolLvlHi = !digitalRead(poolLvlHi_pin);
  poolLvlLo = !digitalRead(poolLvlLo_pin);
}

void readDoorSensors()
{
  poolCabinDoor = !digitalRead(poolCabinDoor_pin);
  poolCabinLocked = !digitalRead(poolCabinLocked_pin);
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- DHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
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
} 

void readInternalDHT()
{
  float temp = dhtInternal.readTemperature();
  float hum = dhtInternal.readHumidity();

  //Output values
  DHTTempIn = isnan(temp) ? initValue : temp;
  DHTHumIn = isnan(hum) ? initValue : hum;

  Sprint("TempIn: ");
  Sprint(DHTTempIn);
  Sprintln("*C");
  Sprint("HumIn: ");
  Sprint(DHTHumIn);
  Sprintln("%");
}

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
