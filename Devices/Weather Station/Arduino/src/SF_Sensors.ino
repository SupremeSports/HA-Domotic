// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ SETUP SENSORS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");

  //INPUTS

  //Wind speed
  #ifndef I2C_WINDSPD
    pinMode(windSpdPin, INPUT_PULLUP);
  #endif
  
  //Wind direction
  pinMode(windDir1Pin, INPUT_PULLUP);
  pinMode(windDir2Pin, INPUT_PULLUP);
  pinMode(windDir3Pin, INPUT_PULLUP);
  pinMode(windDir4Pin, INPUT_PULLUP);
  pinMode(windDir5Pin, INPUT_PULLUP);

  //Rain level
  #ifndef I2C_RAINLVL
    pinMode(rainLevelPin, INPUT_PULLUP);
  #endif

  //ACTIVE SENSORS
  #ifdef EXTERNAL_EN
    dht.begin();
  #endif
  #ifdef INTERNAL_EN  
    dhtInternal.begin();
  #endif

  #ifdef BME_EN  
    if (!bme.begin(I2CAddress_BME))
      Sprintln("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
  #endif

  #ifdef UV_EN
    uv.begin(VEMLTIMECONST);
  #endif

  //OUTPUTS
  if (enableBoardLED)
    pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output

  //pinMode(selfReset_pin, INPUT_PULLUP);                 //Reset pin is an input until ready to use
}

//INPUTS
void readSensors(uint8_t select)
{
  if (select >= 5)
  {
    Sprintln("Read 5 sec");
    readRainSnsr();

    readNTC();
    readDHT();
    readBME();
    readInternalDHT();
  
    readVEML6070();
    readMQ135();
  
    readVoltages();
  }

  if (select >= 1)
  {
    Sprintln("Read 1 sec");
    readWindDir();

    
    #if defined(I2C_WINDSPD) || defined(I2C_RAINLVL)
      readWindRainI2C();
    #endif
    
    #ifndef I2C_WINDSPD
      readWindSpd();
    #endif
  }

  #ifndef I2C_RAINLVL
    readRainLvl();
  #endif
}

//OUTPUTS
void writeOutputs()
{
  return;
}
// ----------------------------------------------------------------------------------------------------
// -------------------------------------- WIND/RAIN I2C SENSORS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
#if defined(I2C_WINDSPD) || defined(I2C_RAINLVL)
void readWindRainI2C()
{
  int available = Wire.requestFrom(I2C_SLAVE_ADDR, I2C_DATA_SIZE);    // request 6 bytes from slave device #8
  long value = 0;
  long configArray[I2C_DATA_SIZE];
  int idx = 0;

  if (available == I2C_DATA_SIZE)
  {
    // IDX - Got protocol header
    byte receivedByte = Wire.read();

    // IDX - I2C Protocol ERROR! Expected header
    if (receivedByte != I2C_PROTOCOL_NUMBER)
      return;

    // IDX 0 - I2C Protocol
    configArray[idx++] = receivedByte;  // Protocol version
    
    // IDX 1 - Rain pulse count
    receivedByte = Wire.read();
    configArray[idx++] = receivedByte;

    // IDX 2-3-4-5 - Wind frequency
    receivedByte = Wire.read();
    configArray[idx++] = receivedByte;
    receivedByte = Wire.read();
    configArray[idx++] = receivedByte;
    receivedByte = Wire.read();
    configArray[idx++] = receivedByte;
    receivedByte = Wire.read();
    configArray[idx++] = receivedByte;

    value = ((configArray[2] << 0) & 0xFF) + ((configArray[3] << 8) & 0xFFFF) + ((configArray[4] << 16) & 0xFFFFFF) + ((configArray[5] << 24) & 0xFFFFFFFF);
  }
  
  #ifdef I2C_RAINLVL
    RainLvlOut += configArray[1]; //Add up values and clear when sent through MQTT
    Sprint("I2C Rain count: ");
    Sprintln(RainLvlOut);
  #endif

  #ifdef I2C_WINDSPD
    windSpdOut = float(value)/100.0;
  
    Sprint("I2C Wind Speed: ");
    Sprint(windSpdOut);
    Sprintln("Hz");
  #endif
}
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WIND SENSORS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifndef I2C_WINDSPD
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
#endif

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
// -------------------------------------------- NTC SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readNTC()
{
  #ifdef NTC_EN
    NTCTempOut = readAn(ntcSensorPin);
    Sprint("NTCOut: ");
    Sprintln(NTCTempOut);
  #else
    NTCTempOut = initValue;
  #endif
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- DHT SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readDHT()
#ifdef EXTERNAL_EN
{
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  //Output values
  DHTTempOut = isnan(temp) ? initValue : temp;
  DHTHumOut = isnan(hum) ? initValue : hum;

  Sprint("TempOut: ");
  Sprint(DHTTempOut);
  Sprintln("*C");
  Sprint("HumOut: ");
  Sprint(DHTHumOut);
  Sprintln("%");
  
  local_delay(1);
} 
#else
{
  DHTTempOut = initValue;
  DHTHumOut = initValue;
}
#endif

void readInternalDHT()
#ifdef INTERNAL_EN
{
  float temp = dhtInternal.readTemperature();
  float hum = dhtInternal.readHumidity();

  //Output values
  DHTTempIn = isnan(temp) ? initValue : temp;
  DHTHumIn = isnan(hum) ? initValue : hum;

  Sprint("TempIn: ");
  Sprint(DHTTempIn);
  Sprintln("*C");
  Sprint("HumIn: ");
  Sprint(DHTHumIn);
  Sprintln("%");

  local_delay(1);
}
#else
{
  DHTTempIn = initValue;
  DHTHumIn = initValue;
}
#endif

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- BME SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readBME()
#ifdef BME_EN
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
#else
{
  BMETempOut = initValue;
  BMEHumOut = initValue;
  BMEBaroOut = initValue;
}
#endif

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- UV SENSOR --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readVEML6070()
#ifdef UV_EN
{  
  VEMLUVOut = uv.readUV();

  if (isnan(VEMLUVOut) || VEMLUVOut<0.0)
    VEMLUVOut = initValue;

  Sprint(VEMLUVOut);
  Sprintln("mW/m2");
}
#else
{
  VEMLUVOut = initValue;
}
#endif

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- AIR QUALITY SENSOR ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void readMQ135()
#ifdef MQ135_EN
{
  int16_t AQI = map(analogRead(MQ135Pin), 0, 1023, 1023, 0);

  Sprint(float(AQI)/1023.0F*5.0F);
  Sprintln("V (air quality)");
  
  MQ135Out = AQI;
}
#else
{
  MQ135Out = initValue;
}
#endif

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- RAIN SENSOR/LEVEL ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void readRainSnsr()
{
  int16_t RainSnsr = readAn(rainSensorPin);

  Sprint(float(RainSnsr)/1023.0F*5.0F);
  Sprintln("V (Rain Sensor)");
  
  RainSnsrOut = RainSnsr;
}

#ifndef I2C_RAINLVL
void readRainLvl()
{
  if (millis()-lastRainLvl >= 60000)
  {
    RainLvlOut = rainLvlBuffer;
    
    rainLvlBuffer = 0;
    lastRainLvl = millis();
  }
  
  byte rainLvlOn = digitalRead(rainLevelPin) ? 1 : 0;
  if (rainLvlOn != rainLvlPrev)
  {
    rainLvlBuffer++;
    rainLvlPrev = rainLvlOn;

    Sprint("Rain Sensor pulse: ");
    Sprintln(rainLvlBuffer);
  }
}
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ VOLTAGE SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readVoltages()
{
  uint16_t voltIn = readAn(voltage12V_pin) * 5;
  uint16_t volt5V = readAn(voltage5V_pin) * 5;

  voltage12V = (voltIn/1023.0F*voltage12VRatio);
  voltage5V = volt5V/1023.0F*voltage5VRatio;

  Sprint(voltage12V);
  Sprintln("Vin");
  Sprint(voltage5V);
  Sprintln("Vout");
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(10);
  return(analogRead(pinToRead));
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- RESET PIN ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void forceReset()
{
  Sprintln("Forced Reset...");
  delay(500);
  resetBoard();
}

void resetBoard()
{
  pinMode(selfReset_pin, OUTPUT);
  digitalWrite(selfReset_pin, LOW);
  delay(10000);
}

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
  if (networkActive)
    flashBoardLed(2, 1);
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
