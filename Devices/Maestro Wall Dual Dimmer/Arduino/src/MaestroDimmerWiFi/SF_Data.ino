void initData()
{
  lamp.fade = true;
  fan.fade = false;
}

void compareData()
{
  if(!newDataNRF && !newDataMQTT && !updateNRF && !updateMQTT && !newStart)
  {
    prepOutput();
    return;
  }

  mqttClient.loop(); //Give time to WiFi/MQTT to run

  if (newDataMQTT || updateMQTT)
  {
    lamp = lampMQTT;
    fan = fanMQTT;
    sendDataNRF = true;
  }
  else if (newDataNRF || updateNRF)
  {
    lamp = lampNRF;
    fan = fanNRF;
  }
  
  lampMQTT = lamp;
  lampNRF = lamp;
  fanMQTT = fan;
  fanNRF = fan;

  prepOutput();

  sendLightState();
  sendFanState();
  lastMinute = millis();
  
  newDataNRF = false;
  updateNRF = false;
  newDataMQTT = false;
  updateMQTT = false;

  sendDataI2C = true;
}

void prepOutput()
{
  uint8_t bufferLvl = 0;
  bufferLvl = lamp.level;
  bitWrite(bufferLvl, STATE_BIT, (lamp.state?1:0));
  bitWrite(bufferLvl, FADE_BIT, (lamp.fade?1:0));
  bitWrite(bufferLvl, FULL_BIT, (lamp.full?1:0));
  lamp.out = bufferLvl;
  
  bufferLvl = fan.level;
  bitWrite(bufferLvl, STATE_BIT, (fan.state?1:0));
  bitWrite(bufferLvl, FADE_BIT, (fan.fade?1:0));
  bitWrite(bufferLvl, FULL_BIT, (fan.full?1:0));
  fan.out = bufferLvl;

  optionData = 0;
  bitWrite(optionData, LED_BIT, (enableRemoteLED?1:0));
}
