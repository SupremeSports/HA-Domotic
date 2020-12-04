// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- INIT PRO MINI ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initArduino()
{
  #ifdef DEBUG
    Serial.begin(BAUDRATE);
    while (!Serial);
  #endif

  checkResetCause();

  pinMode(boardLedPin, OUTPUT);                    // Initialize the LED pin as an output
  setBoardLED(LOW);

  Sprintln("Pro Mini Init Completed!");
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- CHECK RESTART REASON ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkResetCause()
{
  if (MCUSR & (1<<PORF)) // POR
  { 
    Sprintln("Power ON Reset");
    newPowerON = true;
  }
  else
  { 
    Sprintln("Software Reboot");
  }
  MCUSR = 0;
}

// Restarts program from beginning but does not reset the peripherals and registers
void software_Reset() 
{
  asm volatile ("  jmp 0");  
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- WDT SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWDT()
{
  #ifndef ENABLE_WDT
    wdt_disable();
    Sprintln("WDT not enabled");
    return;
  #endif
  wdt_enable(WDT_DELAY);
  Sprint("WDT activated: ");
  Sprint(wdtDelays[WDT_DELAY]);
  Sprintln(WDT_DELAY<6 ? "ms" : "s");
}

void wdtReset()
{
  #ifndef ENABLE_WDT
    wdt_disable();
    return;
  #endif
  wdt_reset();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- LOCAL YIELD/DELAY -----------------------------------------
// ----------------------------------------------------------------------------------------------------
// Need a local yield/delay that that calls yield() and mqttClient.loop()
//   The system yield()/delay() routine does not call mqttClient.loop()

void local_yield()
{
  yield();
  runDimmer();
  wdtReset();
}

void local_delay(unsigned long millisecs)
{
  unsigned long start = millis();
  local_yield();
  if (millisecs > 0)
  {
    while (millis()-start < millisecs)
      local_yield();
  }
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
    setBoardLED(HIGH);
    local_delay(delayFlash);
    setBoardLED(LOW);
    local_delay(delayFlash);
  }
}

//Short flash every 5 seconds when everything is ok
void flashEvery5sec()
{
  flashBoardLed(2, 1);
}
