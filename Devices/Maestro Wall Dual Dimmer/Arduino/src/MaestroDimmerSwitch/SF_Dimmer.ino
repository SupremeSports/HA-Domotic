// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- INIT DIMMERS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initDimmer()
{
  initLamp();
  initFan();

  initPWM();
  
  Sprintln("Dimmers Init Completed");
}

void initLamp()
{
  lamp.level = 1;
  lamp.act = 0;

  lamp.state = false;
  lamp.full = false;
  lamp.fade = true;
}

void initFan()
{
  fan.level = 1;
  fan.act = 0;

  fan.state = false;
  fan.full = false;
  fan.fade = false;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- PWM DIMMERS --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initPWM()
{
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
  
  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);

  for (int i=1; i<15; i++)
    pwm.setPin(i, 4095*PWM_MIN, reversePWM);

  startupShow = true;
}

//Calculates values for each led
void runDimmer()
{
  if (millis()-lastFadeDelay < ledFadeDelay)
    return;

  lastFadeDelay = millis();

  if (startupShow)
  {
    runStartupPWM();
    return;
  }

  runLampDimmer();
  runFanDimmer();
}

void runLampDimmer()
{
  uint8_t bufferLvl = 0.0;

  if (lamp.full)
    bufferLvl = lampMax;
  else if (!lamp.state)
    bufferLvl = 0;
  else
    bufferLvl = lamp.level;
    
  if (lamp.act < bufferLvl)
  {
    lamp.act++;
    lamp.dim = false;
  }
  else if (lamp.act > bufferLvl)
  {
    lamp.act--;
    lamp.dim = false;
  }

  float ledValue = PWM_OFF;

  for (int i=0; i<7; i++)
  {
    ledValue = PWM_OFF;
    if (lamp.state || !lamp.dim)
    {
      if (i+1 == lamp.act)
        ledValue = PWM_ON;
    }
    else
    {
      if (i+1 == lamp.level)
        ledValue = PWM_DIM;
    }
 
    pwm.setPin(pin_lmpLeds[i], 4095*(ledValue)*PWM_OFFSET, reversePWM);
  }

  lamp.dim = lamp.act==bufferLvl;
}

void runFanDimmer()
{
  uint8_t bufferLvl = 0.0;

  if (fan.full)
    bufferLvl = fanMax;
  else if (!fan.state)
    bufferLvl = 0;
  else
    bufferLvl = fan.level;
    
  if (fan.act < bufferLvl)
  {
    fan.act++;
    fan.dim = false;
  }
  else if (fan.act > bufferLvl)
  {
    fan.act--;
    fan.dim = false;
  }

  float ledValue = PWM_OFF;

  for (int i=0; i<7; i++)
  {
    ledValue = PWM_OFF;
    if (fan.state || !fan.dim)
    {
      if (i+1 == fan.act)
        ledValue = PWM_ON;
    }
    else
    {
      if (i+1 == fan.level)
        ledValue = PWM_DIM;
    }
 
    pwm.setPin(pin_fanLeds[i], 4095*(ledValue)*PWM_OFFSET, reversePWM);
  }

  fan.dim = fan.act==bufferLvl;
}

void runStartupPWM()
{
  float ledValue = PWM_OFF;
  for (int i=0; i<7; i++)
  {
    ledValue = PWM_OFF;
    if (fan.state || !fan.dim)
    {
      if (i+1 == startupLevel)
        ledValue = PWM_ON;
    }
 
    pwm.setPin(pin_fanLeds[i], 4095*(ledValue)*PWM_OFFSET, reversePWM);
    pwm.setPin(pin_lmpLeds[i], 4095*(ledValue)*PWM_OFFSET, reversePWM);
  }

  if (startupRev)
    startupLevel--;
  else
    startupLevel++;
    
  if (startupLevel > 7)
  {
    startupLevel -= 2;
    startupRev = true;
  }
  if (startupLevel == 0 && startupRev)
    startupShow = false;
}
