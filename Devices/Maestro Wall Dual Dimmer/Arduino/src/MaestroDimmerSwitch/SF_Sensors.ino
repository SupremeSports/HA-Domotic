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
  for (int i=0; i<6; i++)
    pinMode(buttonPin[i], INPUT_PULLUP);

  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);
  
  Sprintln("Sensors Initiated!");
}

//INPUTS
void readSensors(bool all)
{
  runButtons();

  if (!all)
    return;

  readAnalogSensors();

  #ifdef EXTERNAL_EN
    readDHT();
  #endif
  #ifdef INTERNAL_EN
    readInternalDHT();
  #endif
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- ANALOG SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  readVoltages();
  return;
}

void readVoltages()
{
  #ifndef ENABLE_VOLT
    return;
  #endif
  uint16_t volt5V = readAn(voltage5V_pin) * 5;

  #ifdef ESP32
    voltage5V = volt5V/4095.0F*voltage5VRatio;
  #else
    voltage5V = volt5V/1023.0F*voltage5VRatio;
  #endif
  
  Sprint("5V: ");
  Sprint(voltage5V);
  Sprintln("V");
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  delay(1);
  return(analogRead(pinToRead));
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//TODO

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
  
  DHTTempOut = constrain(DHTTempOut, 0.0, 99.9);
  DHTHumOut = constrain(DHTHumOut, 0.0, 99.9);

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

  //Output values
  DHTTempIn = isnan(temp) ? initValue : temp;
  DHTHumIn = isnan(hum) ? initValue : hum;

  DHTTempOut = constrain(DHTTempOut, 0.0, 99.9);
  DHTHumOut = constrain(DHTHumOut, 0.0, 99.9);

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
