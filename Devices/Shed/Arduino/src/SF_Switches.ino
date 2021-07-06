// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SWITCHES FUNCTIONS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSwitches()
{
  Sprintln("Init Switches...");
  
  toggleSw[0].swPin = lightIntSwPin;
  toggleSw[0].outPtr = lightInt_ptr;

  pinMode(toggleSw[0].swPin, INPUT_PULLUP);
  toggleSw[0].swStts = !digitalRead(toggleSw[0].swPin);
  toggleSw[0].swPrev = toggleSw[0].swStts;

  relayCmd[toggleSw[0].outPtr] = toggleSw[0].state;
}

//Toggling light for every state change
bool readLightToggles(uint8_t ptr)
{
  toggleSw[ptr].swStts = !digitalRead(toggleSw[ptr].swPin);

  if (toggleSw[ptr].swPrev != toggleSw[ptr].swStts)
  {
    relayCmd[toggleSw[ptr].outPtr] = !relayCmd[toggleSw[ptr].outPtr];
    toggleSw[ptr].swPrev = toggleSw[ptr].swStts;
    
    forceMqttUpdate = true;
  }

  toggleSw[ptr].state = relayCmd[toggleSw[ptr].outPtr];
}
