// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- INIT MILIGHT COLOR ----------------------------------------
// ----------------------------------------------------------------------------------------------------
void initPostal()
{
  Sprintln("Init Postal Alarm...");

  initEEPROM();

  initLora();
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- PROCESS POSTAL JSON ---------------------------------------
// ----------------------------------------------------------------------------------------------------
//JSON version to be implmented shortly
//bool processPostalJson(char* message)
//{
//  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
//
//  JsonObject& root = jsonBuffer.parseObject(message);
//
//  if (!root.success())
//  {
//    Sprintln("parseObject() failed");
//    return false;
//  }
//
//  if (root.containsKey("state"))
//  {
//    if (strcmp(root[json_state], mqtt_cmdOn) == 0)
//      mailPresent = true;
//    else if (strcmp(root[json_state], mqtt_cmdOff) == 0)
//      mailPresent = false;
//  }  
//  return true;
//}

void sendPostalState()
{
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  char charVcc[5];
  char charBat[5];
  char charId[5];
  char charRssi[5];

  Vcc.toCharArray(charVcc, 5);
  Bat.toCharArray(charBat, 5);
  Id.toCharArray(charId, 5);
  rssiValue.toCharArray(charRssi, 5);

  root[json_state] = mailPresent ? mqtt_cmdOn : mqtt_cmdOff;
  root[json_postalVcc] = charVcc;
  root[json_postalBat] = charBat;
  root[json_postalId] = charId;
  root[json_postalRssi] = charRssi;

  char buffer[root.measureLength() + 1];
  root.printTo(buffer, sizeof(buffer));

  mqttClient.publish(mqtt_postalStts, buffer, true);
}
