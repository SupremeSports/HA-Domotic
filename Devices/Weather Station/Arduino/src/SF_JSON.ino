// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void processCommandJson(char* message)
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error)
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (doc[json_reset])
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(doc[json_reset], mqtt_cmdOn) != 0)
      return;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);

    if (ENABLE_WDT)
      forceReset();
  }
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- PROCESS JSON SENSORS ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendDigAnStates()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  JsonObject wind = doc.createNestedObject("wind");
  #ifdef I2C_WINDSPD
    dtostrf(windSpdOut, 1, 2, result);
  #else
    dtostrf(windSpdOut, 1, 0, result);
  #endif
  wind["s"] = result;
  dtostrf(windDirOut, 1, 0, result);
  wind["d"] = result;

  JsonObject ntc = doc.createNestedObject("ntc");
  dtostrf(NTCTempOut, 1, 0, result);
  ntc["t"] = result;

  JsonObject dht = doc.createNestedObject("dht");
  dtostrf(DHTTempOut, 1, 1, result);
  dht["t"] = result;
  dtostrf(DHTHumOut, 1, 1, result);
  dht["h"] = result;

  JsonObject bme = doc.createNestedObject("bme");
  dtostrf(BMETempOut, 1, 1, result);
  bme["t"] = result;
  dtostrf(BMEHumOut, 1, 1, result);
  bme["h"] = result;
  dtostrf(BMEBaroOut, 1, 1, result);
  bme["p"] = result;

  JsonObject rain = doc.createNestedObject("rain");
  dtostrf(RainSnsrOut, 1, 0, result);
  rain["s"] = result;
  dtostrf(RainLvlOut, 1, 0, result);
  rain["l"] = result;
  #ifdef I2C_RAINLVL
    RainLvlOut = 0;
  #else
    rain["t"] = rainLvlPrev==0 ? mqtt_cmdOff : mqtt_cmdOn; //Is bucket ON or OFF
  #endif

  dtostrf(VEMLUVOut, 1, 0, result);
  root[json_uv] = result;                 //Raw value [0 => 9999]mW/m2

  dtostrf(MQ135Out, 1, 0, result);
  root[json_mq135] = result;              //Raw value [0 => 9999]ppm

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);
  
  mqttClient.publish(mqtt_sensorsStts, buffer);
  mqttClient.loop();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  StaticJsonDocument<BUFFER_SIZE> doc;
  JsonObject root = doc.to<JsonObject>();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = mqtt_cmdOn;
  root[json_version] = version;
  root[json_date] = date;

#ifdef INTERNAL_EN
  dtostrf(DHTTempIn, 1, 1, result);
  root[json_tempin] = result;

  dtostrf(DHTHumIn, 1, 1, result);
  root[json_humin] = result;
#endif
//External DHT is not a room sensor but used for weather station
//#ifdef EXTERNAL_EN
//  dtostrf(DHTTempOut, 1, 1, result);
//  root[json_tempout] = result;
//
//  dtostrf(DHTHumOut, 1, 1, result);
//  root[json_humout] = result;
//#endif

  mqttClient.loop();
  
  dtostrf(voltage12V, 1, 1, result);
  root[json_12v] = result;

  dtostrf(voltage5V, 1, 1, result);
  root[json_5v] = result;

  #if defined(ESP32) || defined(ESP8266)
    root[json_ssid] = WiFi.SSID();
    
    dtostrf(rssi, 1, 2, result);
    root[json_rssi] = result;
  
    dtostrf(rssiPercent, 1, 2, result);
    root[json_rssiPercent] = result;
  #endif

  char buffer[BUFFER_ARRAY_SIZE];
  serializeJson(doc, buffer);

  mqttClient.publish(mqtt_sensorJson, buffer);
  mqttClient.loop();
}
