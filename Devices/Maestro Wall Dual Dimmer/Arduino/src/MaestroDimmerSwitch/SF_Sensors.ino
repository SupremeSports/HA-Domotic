// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

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
    setBoardLED(!boardLedPinRevert);
    local_delay(delayFlash);
    setBoardLED(boardLedPinRevert);
    local_delay(delayFlash);
  }
}
