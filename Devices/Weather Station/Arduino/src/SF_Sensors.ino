// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ SETUP SENSORS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  dhtExternal.begin();
  dhtInternal.begin();

  if (!bme.begin(I2CAddress_BME))
    Sprintln("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");

  uv.begin(VEMLTIMECONST);

  initWind();
}

void initWind()
{
  Sprintln("Init Wind Sensors...");
  //Wind speed
  #ifdef FREQLIBRARIES
    FreqMeasure.begin();
  #endif
  pinMode(windSpdPin, INPUT_PULLUP);
  
  //Wind direction
  pinMode(windDir1Pin, INPUT_PULLUP);
  pinMode(windDir2Pin, INPUT_PULLUP);
  pinMode(windDir3Pin, INPUT_PULLUP);
  pinMode(windDir4Pin, INPUT_PULLUP);
  pinMode(windDir5Pin, INPUT_PULLUP);

  //Rain level
  pinMode(rainLevelPin, INPUT_PULLUP);

  //Reset pin is an input until ready to use
  pinMode(selfResetPin, INPUT_PULLUP);
}

//INPUTS
void readSensors(bool all)
{
  readWindSpd();
  readWindDir();

  readRainLvl();

  if (!all)
    return;

  readRainSnsr();

  readDHT();
  readBME();
  readInternalDHT();

  readVEML6070();
  readMQ135();

  readVoltages();
}

//OUTPUTS
void writeOutputs()
{
  return;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WIND SENSORS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readWindSpd()
{
  if (millis()-lastWindSpeed >= 1000)
  {
    windSpdOut = windSpeedBuffer;
    
    windSpeedBuffer = 0;
    lastWindSpeed = millis();
  }
  
  byte windSpeedOn = digitalRead(windSpdPin) ? 1 : 0;
  if (windSpeedOn != windSpeedPrev)
  {
    windSpeedBuffer++;
    windSpeedPrev = windSpeedOn;
  }
}

void readWindDir()
{
  int def = initValue;
  
  uint16_t offsetAngle = WindDirOffset;   //Offset to apply to angle value
  uint8_t tableSize = 32;
  uint16_t angleTable[tableSize] = 
                    {
                      def,   0, 216, 204,  72,  12,  60,  24,
                      144, 348, 156, 168,  84, 336,  48,  36,
                      288, 300, 228, 192, 276, 264, 240, 252,
                      132, 312, 120, 180,  96, 324, 108, def
                    };
  bool windDir[5] = {
                      digitalRead(windDir1Pin),
                      digitalRead(windDir2Pin),
                      digitalRead(windDir3Pin),
                      digitalRead(windDir4Pin),
                      digitalRead(windDir5Pin)
                    };

  //Find gray code ID
  uint8_t dirID = 0;
  for (int i=0; i<5; i++)
  {
    dirID |= (windDir[i]?1:0) << i;
    Sprint(windDir[i]);
  }
  Sprintln();

  dirID = constrain(dirID, 0, tableSize-1);
  
  //Find corresponding angle in table
  uint16_t dirAngle = angleTable[dirID];

  if (dirAngle < 0)
    return;

    //Add offset and make sure it stays in range
  dirAngle += offsetAngle;
  if (dirAngle < 0)
    dirAngle += 360;
  if (dirAngle > 360)
    dirAngle -= 360;

  dirAngle = constrain(dirAngle, 0, 360);

  Sprint(dirAngle);
  Sprintln("*");
  
  windDirOut = dirAngle;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- DHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readDHT()
{
  float temp = dhtExternal.readTemperature();
  float hum = dhtExternal.readHumidity();

  if (isnan(temp))
  {
    Sprintln("DHT read error");
    temp = initValue;
  }
  else
  {
    Sprint(temp);
    Sprintln("*C");
  }

  if (isnan(hum))
  {
    Sprintln("DHT read error");
    hum = initValue;
  }
  else
  {
    Sprint(hum);
    Sprintln("%");
  }

  //Output values
  DHTTempOut = temp;
  DHTHumOut = hum;
} 

void readInternalDHT()
{
  float temp = dhtInternal.readTemperature();
  float hum = dhtInternal.readHumidity();

  if (isnan(temp))
  {
    Sprintln("Internal DHT read error");
    temp = initValue;
  }
  else
  {
    Sprint(temp);
    Sprintln("*C");
  }

  if (isnan(hum))
  {
    Sprintln("Internal DHT read error");
    hum = initValue;
  }
  else
  {
    Sprint(hum);
    Sprintln("%");
  }

  //Output values
  DHTTempIn = temp;
  DHTHumIn = hum;
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- BME SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readBME()
{
  float temp = bme.readTemperature();
  float hum = bme.readHumidity();
  float baro = bme.readPressure() / 1000.0F; //Value in Pascals (Pa)

  BMEreadFailed = (isnan(temp) && isnan(hum) && isnan(baro));

  if (isnan(temp))
    temp = initValue;
  if (isnan(hum))
    hum = initValue;
  if (isnan(baro))
    baro = initValue;

  Sprint(temp);
  Sprintln("*C");
  Sprint(hum);
  Sprintln("%");
  Sprint(baro);
  Sprintln("kPa");

  //Output values
  BMETempOut = temp;
  BMEHumOut = hum;
  BMEBaroOut = baro;
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- UV SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readVEML6070()
{
  VEMLUVOut = uv.readUV();

  if (isnan(VEMLUVOut) || VEMLUVOut<0.0)
    VEMLUVOut = initValue;

  Sprint(VEMLUVOut);
  Sprintln("mW/m2");
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- AIR QUALITY SENSOR ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void readMQ135()
{
  int16_t AQI = map(analogRead(MQ135Pin), 0, 1023, 1023, 0);

  Sprint(float(AQI)/1023.0F*5.0F);
  Sprintln("V (air quality)");
  
  MQ135Out = AQI;
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- RAIN SENSOR/LEVEL ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void readRainSnsr()
{
  int16_t RainSnsr = analogRead(rainSensorPin);

  Sprint(float(RainSnsr)/1023.0F*5.0F);
  Sprintln("V (Rain Sensor)");
  
  RainSnsrOut = RainSnsr;
}

void readRainLvl()
{
  float millisLvlCounter = millis()-rainLvlMillis;
  float bucketsPerHour = float(rainLvlCounter) * (millisLvlCounter/1000.0F) * 60.0F;

//  Sprint(millisLvlCounter);
//  Sprintln("ms");
//  Sprint(bucketsPerHour);
//  Sprintln("buckets/min");
  
  RainLvlOut = bucketsPerHour;

  //Reset values for next reading
  rainLvlCounter = 0;
  rainLvlMillis = millis();
}

void readRainLvlFreq()
{
  RainLvlStts = digitalRead(rainLevelPin);

  if ((rainLvlMemory != RainLvlStts))
    rainLvlCounter++;

  rainLvlMemory = RainLvlStts;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ VOLTAGE SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readVoltages()
{
  analogRead(inputVoltPin); //Dump first reading
  uint16_t voltIn = analogRead(inputVoltPin) * 5;
  delay(10);
  analogRead(dcVoltPin); //Dump first reading
  uint16_t volt5V = analogRead(dcVoltPin) * 5;

  inputVoltOut = (voltIn/1023.0F*inputVoltRatio);
  dcVoltOut = volt5V/1023.0F*dcVoltRatio;

  Sprint(inputVoltOut);
  Sprintln("Vin");
  Sprint(dcVoltOut);
  Sprintln("Vout");
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- RESET PIN ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void selfReset()
{
  if (BMEreadFailed)
  {
    if (!resetONS)
    {
      resetDelay = millis();
      resetONS = true;
      Sprintln("Self Reset ONS");
    }
  }
  else
    resetONS = false;

  if(resetONS && (millis() - resetDelay) > 5000)
  {
    mqttClient.publish(mqtt_controlStts, "Self reset...");
    Sprintln("Self Reset...");
    delay(500);
    resetBoard();
  }
}

void forceReset()
{
  Sprintln("Forced Reset...");
  delay(500);
  resetBoard();
}

void resetBoard()
{
  pinMode(selfResetPin, OUTPUT);
  digitalWrite(selfResetPin, LOW);
  delay(10000);
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
