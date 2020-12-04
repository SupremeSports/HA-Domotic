// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- INIT DIMMERS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initDimmer()
{
  DimmableLight::setSyncPin(ZC_PIN);
  DimmableLight::setSyncDir(FALLING); //Default = RISING
  DimmableLight::setSyncPullup(true); //Default = false

  //300   Original value, but flickers
  //470   If used with LED in series with MOC3021
  //1000  If used without LED in series with MOC3021
  DimmableLight::setGateTurnOffTime(1000); //Default = 300
  DimmableLight::begin();

  initLamp();
  initFan();

  wdtReset();

  Sprintln("Dimmers Init Completed");
}

void initLamp()
{
  lamp.level = 1;
  
  lamp.table[0] = 0;    //Full OFF
  lamp.table[1] = 40;
  lamp.table[2] = 46;
  lamp.table[3] = 50;
  lamp.table[4] = 55;
  lamp.table[5] = 60;
  lamp.table[6] = 70;
  lamp.table[7] = 85;
  lamp.table[8] = 100;  //Full ON

  lamp.rate = 0.005;

  lamp.state = false;
  lamp.full = false;
  lamp.fade = false;
}

void initFan()
{
  fan.level = 1;
  
  fan.table[0] = 0;    //Full OFF
  fan.table[1] = 40;
  fan.table[2] = 46;
  fan.table[3] = 50;
  fan.table[4] = 55;
  fan.table[5] = 60;
  fan.table[6] = 70;
  fan.table[7] = 85;
  fan.table[8] = 100;  //Full ON

  fan.rate = 0.005;

  fan.state = false;
  fan.full = false;
  fan.fade = false;
}
// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- RUN DIMMERS --------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Calculates delays for triggering the TRIACs
void runDimmer()
{
  wdtReset();
  
  float bufferLvl = 0.0;
  //Lamp control timing using interrupt timerOne
  bufferLvl = lamp.table[lamp.state ? lamp.level : 0];

  if (lamp.full)
    bufferLvl = lamp.table[8];
    
  if (!lamp.fade)
    lamp.act = bufferLvl;
  else if (lamp.act > bufferLvl-1 && lamp.act < bufferLvl+1)
    lamp.act = bufferLvl;
  else if (lamp.act < bufferLvl)
  {
    //Prevent fading taking too long to start lighting as first half is not efficient
    if (lamp.act<lamp.table[1]/2)
      lamp.act = lamp.table[1]/2;
    else
      lamp.act += lamp.rate;
  }
  else if (lamp.act > bufferLvl)
    lamp.act -= lamp.rate;

  if (lamp.act < lamp.table[1]/2)
    LAMP.setBrightness(0);
  else
    LAMP.setBrightness(map(lamp.act, 0, 100, 0, 255));

  //Fan control timing using interrupt timerOne
  bufferLvl = fan.table[fan.state ? fan.level : 0];
  
  if (fan.full)
    bufferLvl = fan.table[8];
    
  if (!fan.fade)
    fan.act = bufferLvl;
  else if (fan.act > bufferLvl-1 && fan.act < bufferLvl+1)
    fan.act = bufferLvl;
  else if (fan.act < bufferLvl)
  {
    //Prevent fading taking too long to start lighting as first half is not efficient
    if (fan.act<fan.table[1]/2)
      fan.act = fan.table[1]/2;
    else
      fan.act += fan.rate;
  }
  else if (fan.act > bufferLvl)
    fan.act -= fan.rate;

  if (fan.act < fan.table[1]/2)
    FAN.setBrightness(0);
  else
    FAN.setBrightness(map(fan.act, 0, 100, 0, 255));
}
