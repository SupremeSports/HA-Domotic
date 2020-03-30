// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  pinMode(limitSwitch1Pin, INPUT);
  pinMode(limitSwitch2Pin, INPUT);
  pinMode(limitSwitch3Pin, INPUT);

  pinMode(doorReedPin, INPUT_PULLUP);
  pinMode(doorbellPin, INPUT_PULLUP);

  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
  
  pinMode(Motor1OutputPin, OUTPUT);
  pinMode(Motor2OutputPin, OUTPUT);

  digitalWrite(Motor1OutputPin, HIGH);
  digitalWrite(Motor2OutputPin, HIGH);
}

//INPUTS
void readSensors()
{
  limitSwitch1 = digitalRead(limitSwitch1Pin);
  limitSwitch2 = digitalRead(limitSwitch2Pin);
  limitSwitch3 = SOLID_GEAR ? digitalRead(limitSwitch3Pin) : false;

  doorStatus = !digitalRead(doorReedPin);

  readDoorbell();

  readLockStatus();
}

//OUTPUTS
void writeOutputs()
{  
  digitalWrite(Motor1OutputPin, Motor1Output);
  digitalWrite(Motor2OutputPin, Motor2Output);
}

void readLockStatus()
{
  locked = false;
  unlocked = false;

  #if SOLID_GEAR
    if (!limitSwitch1 && !limitSwitch2)
      locked = true;
    else if (limitSwitch1 && !limitSwitch2)
      unlocked = true;

    //Sprintln(limitSwitch3 ? "Not in position" : "In position");
  #else
    if (limitSwitch1)
      locked = true;
    else if (limitSwitch2)
      unlocked = true;
  #endif

  //If not locking on the right, invert locked and unlocked status
  if (!LOCKED_ON_RIGHT)
  {
    locked = !locked;
    unlocked = !unlocked;
  }

  if ((!locked && !unlocked) || (locked && unlocked))
  {
    locked = false;
    unlocked = false;
  }

  lockStatus = locked;
}

void readVoltage()
{
  int percent = map(analogRead(voltPin), 768, 950, 0, 100);
  percent = constrain(percent, 0, 100);

  batStatus = constrain(percent, 0, 100);
}

void readDoorbell()
{
  bool doorbellPressed = !digitalRead(doorbellPin);
  
  if (doorbellStatus && doorbellPressed)
    return;

  if (doorbellStatus && !doorbellPressed)
  {
    doorbellStatus = false;
    lastSecond = millis()-1000;
    return;
  }

  if (!doorbellStatus && doorbellPressed)
  {
    doorbellStatus = true;
    lastSecond = millis()-1000;
    return;
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

  //readSensors();
  readVoltage();
  sendSensors();
}
