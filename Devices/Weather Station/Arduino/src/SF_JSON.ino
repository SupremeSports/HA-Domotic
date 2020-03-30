// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT JSON ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
//NONE FOR THIS PROJECT

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- PROCESS CMD JSON -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void processCommandJson(char* message)
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(message);

  if (!root.success())
  {
    Sprintln(json_parseFailed);
    return;
  }

  if (root.containsKey(json_reset))
  {
    mqttClient.publish(mqtt_controlStts, json_resetReq);
    
    if (strcmp(root[json_reset], mqtt_cmdOn) != 0)
      return;
  
    mqttClient.publish(mqtt_controlStts, json_resetReboot);
      
    Sprint(json_resetReq);
    local_delay(500);
    Sprintln(json_resetReboot);

    forceReset();
  }
}

void sendCommandStatus()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  JsonObject& wind = root.createNestedObject("wind");
  dtostrf(windSpdOut, 4, 0, result);    // Leave room for too large numbers!
  wind["s"] = result;
  dtostrf(windDirOut, 3, 0, result);    // Leave room for too large numbers!
  wind["d"] = result;

  JsonObject& dht = root.createNestedObject("dht");
  dtostrf(DHTTempOut, 5, 1, result);    // Leave room for too large numbers!
  dht["t"] = result;
  dtostrf(DHTHumOut, 5, 1, result);     // Leave room for too large numbers!
  dht["h"] = result;
  
  JsonObject& bme = root.createNestedObject("bme");
  dtostrf(BMETempOut, 5, 1, result);    // Leave room for too large numbers!
  bme["t"] = result;
  dtostrf(BMEHumOut, 5, 1, result);     // Leave room for too large numbers!
  bme["h"] = result;
  dtostrf(BMEBaroOut, 5, 1, result);    // Leave room for too large numbers!
  bme["p"] = result;

  JsonObject& rain = root.createNestedObject("rain");
  dtostrf(RainSnsrOut, 4, 0, result); // Leave room for too large numbers!
  rain["s"] = result;
  dtostrf(RainLvlOut, 3, 0, result);    // Leave room for too large numbers!
  rain["l"] = result;
  rain["t"] = RainLvlStts ? mqtt_cmdOff : mqtt_cmdOn; //Is bucket ON or OFF

  dtostrf(VEMLUVOut, 4, 0, result);     // Leave room for too large numbers!
  root[json_uv] = result;                 //Raw value [0 => 9999]mW/m2

  dtostrf(MQ135Out, 4, 0, result);      // Leave room for too large numbers!
  root[json_mq135] = result;              //Raw value [0 => 9999]ppm

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_controlStts, buffer);
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SEND JSON SENSORS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void sendSensors()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  uint8_t chrLngt = 8;  // Buffer big enough for 7-character float
  char result[chrLngt];

  root[json_state] = mqtt_cmdOn;

  dtostrf(DHTTempIn, 5, 1, result);     // Leave room for too large numbers!
  root[json_temp] = result;

  dtostrf(DHTHumIn, 5, 1, result);     // Leave room for too large numbers!
  root[json_hum] = result;
  
  dtostrf(inputVoltOut, 4, 1, result);  // Leave room for too large numbers!
  root[json_vin] = result;

  dtostrf(dcVoltOut, 4, 1, result);     // Leave room for too large numbers!
  root[json_vcc] = result;

  char buffer[BUFFER_ARRAY_SIZE];
  root.printTo(buffer, root.measureLength() + 1);

  mqttClient.publish(mqtt_sensorJson, buffer);
}
