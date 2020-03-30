// ----------------------------------------------------------------------------------------------------
// ------------------------------------ DOOR LOCK/UNLOCK FUNCTIONS ------------------------------------
// ----------------------------------------------------------------------------------------------------
void doorLockRequest(bool lock)
{
  Sprint(lock ? "Lock" : "Unlock");
  Sprintln(" Request Received");

  lockRequest = false;
  unlockRequest = false;

  //Lock has to be in valid motor position to activate a lock or unlock request
  if (!limitSwitch3)
  {
    lockRequest = lock;
    unlockRequest = !lock;
    newRequest = true;

    fallingEdgeLS3 = limitSwitch3;
  }
//  else if (initialPublish)
//  {
//    Sprintln("Request aborted - Initial Publish");
//
//    initialPublish = false;
//  }
  else
    Sprintln("Request aborted - Invalid position of lock");

  //Protect motor if it is already locked
  if (lockRequest && locked)
  {
    lockRequest = false;
    unlockRequest = false;
    newRequest = false;
  }

  //Protect motor if it is already unlocked
  if (unlockRequest && unlocked)
  {
    lockRequest = false;
    unlockRequest = false;
    newRequest = false;
  }

  runTimeMotor = millis();
  reverseTimeMotor = millis() - minRevMotor;
  holdTimeMotor = millis();
}

void doorLockMode(bool lockOnRight)
{
  LOCKED_ON_RIGHT = lockOnRight;
}

void moveMotor()
{
  bool waitReverseTime = millis() - reverseTimeMotor >= minRevMotor;

  if (lockRequest || (middleRequestUnlock && waitReverseTime))
  {
    Motor1Output = LOCKED_ON_RIGHT;
    Motor2Output = !LOCKED_ON_RIGHT;
  }
  else if (unlockRequest || (middleRequestLock && waitReverseTime))
  {
    Motor1Output = !LOCKED_ON_RIGHT;
    Motor2Output = LOCKED_ON_RIGHT;
  }
  else
  {
    Motor1Output = false;
    Motor2Output = false;
  }
}

//Returns false when everything is OK
boolean powerUp()
{
  if (!newStart || !SOLID_GEAR)
    return false;

  wdtReset();
  runOTA();

  return digitalRead(limitSwitch3Pin);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SOLID GEAR VERSION ----------------------------------------
// ----------------------------------------------------------------------------------------------------
#if SOLID_GEAR
void doorLockLoop()
{
  if (!newRequest)
    runTimeMotor = millis();

  bool fallingEdgeDetection = ((fallingEdgeLS3 != limitSwitch3) && !limitSwitch3);

  if (fallingEdgeDetection)
  {
    bool stopExcessTime = millis() - runTimeMotor >= maxTimeMotor;

    fallingEdgeLS3 = limitSwitch3;

    Sprintln("Falling Edge Detected");

    //If delay expired, stop all movement
    if (stopExcessTime)
    {
      lockRequest = false;
      unlockRequest = false;
      middleRequestLock = false;
      middleRequestUnlock = false;
      newRequest = false;
    }

    //On lock request, stop motor if it is already locked or if the LS3 reaches the max position
    // Returns to clear the falling edge detection and give a delay between direction change
    else if (lockRequest)
    {
      lockRequest = false;
      unlockRequest = false;
      middleRequestLock = true;
      reverseTimeMotor = millis();
      Sprintln("End Lock Loop");
      return;
    }

    //On unlock request, stop motor if it is already unlocked or if the LS3 reaches the max position
    // Returns to clear the falling edge detection and give a delay between direction change
    else if (unlockRequest)
    {
      lockRequest = false;
      unlockRequest = false;
      middleRequestUnlock = true;
      reverseTimeMotor = millis();
      Sprintln("End Unlock Loop");
      return;
    }

    else if (middleRequestUnlock)
    {
      middleRequestUnlock = false;
      newRequest = false;
      Sprintln("End Middle Loop");
      return;
    }
    else if (middleRequestLock)
    {
      middleRequestLock = false;
      newRequest = false;
      Sprintln("End Middle Loop");
      return;
    }
  }

  moveMotor();

  fallingEdgeLS3 = limitSwitch3;
}
// ----------------------------------------------------------------------------------------------------
// -------------------------------------- NON-SOLID GEAR VERSION --------------------------------------
// ----------------------------------------------------------------------------------------------------
#else
void doorLockLoop()
{
  if (!newRequest)
    runTimeMotor = millis();

  bool stopExcessTime = millis() - runTimeMotor >= maxTimeMotor;

  //If delay expired, stop all movement
  if (stopExcessTime || millis()-holdTimeMotor > 750)
  {
    lockRequest = false;
    unlockRequest = false;
    middleRequestLock = false;
    middleRequestUnlock = false;
    newRequest = false;
  }

  if (lockRequest)
  {
    if (locked)
    {
      lockRequest = false;
      unlockRequest = false;
      middleRequestUnlock = true;
      Sprintln("End Lock Loop");
      return;
    }
    else
      holdTimeMotor = millis();
  }

  if (unlockRequest)
  {
    if (unlocked)
    {
      lockRequest = false;
      unlockRequest = false;
      middleRequestLock = true;
      holdTimeMotor = millis();
      Sprintln("End Unlock Loop");
      return;
    }
    else
      holdTimeMotor = millis();
  }

  moveMotor();
}
#endif
