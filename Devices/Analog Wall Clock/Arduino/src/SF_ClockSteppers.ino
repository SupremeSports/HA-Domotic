// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- CLOCK CALLBACKS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initClock()
{
  if (clockHomed || updatePublish)
    return;
    
  Sprintln("Init Clock...");

  while (!findZero(STEPPER_ALL))
    local_delay(10);

  local_delay(10);

  updatePublish = true;
}

void updateClock()
{
  if (!clockHomed || !localTimeValid)
    return;
    
  updateHands();        //Every second
  checkTime();          //Every hour, insure the hands are in position
}
//Turn hands until it is at 12:00:00
boolean findZero(int hand)
{
  Sprintln("Homing Clock...");
  //Set them all to true in case one of the stepper is undefined, it will return OK
  bool HourHallSignal = true;
  bool MinHallSignal = true;
  bool SecHallSignal = true;

  #ifdef HOURS
    if (hand == STEPPER_ALL || hand == STEPPER_HOUR)
    {
      HourHallSignal = readHallSensor(HourHall);
      while (!HourHallSignal || !initialHourOns)
      {
        tickStepper(STEPPER_HOUR);
        HourHallSignal = readHallSensor(HourHall);
        if (!HourHallSignal)
          initialHourOns = true;
      }
      
      HourTick = 0;
    }
  #endif

  #ifdef MINS
    if (hand == STEPPER_ALL || hand == STEPPER_MIN)
    {
      MinHallSignal = readHallSensor(MinHall);
      while (!MinHallSignal || !initialMinOns)
      {
        tickStepper(STEPPER_MIN);
        MinHallSignal = readHallSensor(MinHall);
        if (!MinHallSignal)
          initialMinOns = true;
      }
      
      MinTick = 0;
    }
  #endif

  #ifdef SECS
    if (hand == STEPPER_ALL || hand == STEPPER_SEC)
    {
      SecHallSignal = readHallSensor(SecHall);
      while (!SecHallSignal || !initialSecOns)
      {
        tickStepper(STEPPER_SEC);
        SecHallSignal = readHallSensor(SecHall);
        if (!SecHallSignal)
          initialSecOns = true;
      }

      SecTick = 0;
    }
  #endif

  if (HourHallSignal && MinHallSignal && SecHallSignal)   
  {
    Sprintln("Find zero complete");
    clockHomed = true;
    
    lastMillis = millis();
    
    while (millis() - lastMillis < 2000)
      flashBoardLed(100, 1);

    return true;
  }

  Sprintln("Find zero failed");
  return false;
}

//Adjust hour and minute hands according to current time
//Hour and Minute updated once per second
void updateHands()
{
  //If time data is not within limits, return
  //If second has not changed, return
  if (Hour < 0 || Hour > 23 || Minute < 0 || Minute > 59 || Second < 0 || Second > 59 || Second == prevSec || Second%5!=0)
    return;

  #ifdef HOURS
    uint16_t HourTickTarget = ((Hour % 12) * (PULSE_PER_REV/12)) + (Minute * ((PULSE_PER_REV/12)/60));
    Sprint("Adjusting Hours: ");
    Sprint(HourTickTarget);
    Sprintln(" pulses");
    
    while (HourTickTarget != HourTick)
      tickStepper(STEPPER_HOUR);

    Sprintln("Adjust Hours Completed...");
  #endif

  #ifdef MINS
    uint16_t MinTickTarget = (Minute * (PULSE_PER_REV/60)) + ((Second * PULSE_PER_REV) / (60*60));
    Sprint("Adjusting Minutes: ");
    Sprint(MinTickTarget);
    Sprintln(" pulses");
    
    while (MinTickTarget != MinTick)
      tickStepper(STEPPER_MIN);

    Sprintln("Adjust Minutes Completed...");
  #endif

  #ifdef SECS      
    uint16_t SecTickTarget = Second * (PULSE_PER_REV/60);
    Sprint("Adjusting Seconds: ");
    Sprint(SecTickTarget);
    Sprintln(" pulses");
    while (SecTickTarget != SecTick)
      tickStepper(STEPPER_SEC);

    Sprintln("Adjust Seconds Completed...");
  #endif

  prevSec = Second;
}

//Check if hands are at zero when supposed to be
void checkTime()
{
  //Set them all to true in case one of the stepper is undefined, it will return OK
  bool HourHallSignal = true;
  bool MinHallSignal = true;
  bool SecHallSignal = true;
  
  #ifdef HOURS
    HourHallSignal = readHallSensor(HourHall);
    if (!HourHallSignal && ((Hour % 12) == 0) && Minute < 2)
      findZero(STEPPER_HOUR);  
  #endif
  
  #ifdef MINS
    MinHallSignal = readHallSensor(MinHall);
    if (!MinHallSignal && Minute == 0 && Second < 2)
      findZero(STEPPER_MIN);
  #endif
  
  #ifdef SECS
    SecHallSignal = readHallSensor(SecHall);
    if (!SecHallSignal && Second == 0)
      findZero(STEPPER_SECD);
  #endif
}

//Tick stepper motor by one pulse
void tickStepper(int motor)
{
  int handPulsePin = 0;
  
  if (motor == STEPPER_HOUR)
  {
    handPulsePin = HourPulse;

    HourTick += 1;
    if (HourTick >= PULSE_PER_REV)
      HourTick = 0;
    if (HourTick < 0)
      HourTick = PULSE_PER_REV;

    Sprint("Pulse Hour: ");
    Sprintln(HourTick);
  }
  else if (motor == STEPPER_MIN)
  {
    handPulsePin = MinPulse;

    MinTick += 1;
    if (MinTick >= PULSE_PER_REV)
      MinTick = 0;
    if (MinTick < 0)
      MinTick = PULSE_PER_REV;

    Sprint("Pulse Minute: ");
    Sprintln(MinTick);
  }
  else if (motor == STEPPER_SEC)
  {
    handPulsePin = SecPulse;

    SecTick += 1;
    if (SecTick >= PULSE_PER_REV)
      SecTick = 0;
    if (SecTick < 0)
      SecTick = PULSE_PER_REV;

    Sprint("Pulse Second: ");
    Sprintln(SecTick);
  }
  else
    return;

  uint16_t delayPulse = clockHomed ? STEPPER_SPEED : STEPPER_SPEED/2;

  digitalWrite(StepEnable, LOW); 
  delayMicroseconds(delayPulse);
  digitalWrite(handPulsePin, HIGH);
  delayMicroseconds(delayPulse);
  digitalWrite(handPulsePin, LOW);
  delayMicroseconds(delayPulse);
  digitalWrite(StepEnable, HIGH); 

  local_delay(2);
  wdtReset();
}

boolean readHallSensor(int hallPin)
{
  return (!digitalRead(hallPin));
}
