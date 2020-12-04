// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- I2C SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initI2C()
{
  Wire.begin(); // join i2c bus (address optional for master)
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- I2C RUN ----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void runI2C()
{
  if ((millis()-lastDataI2C < 1000 && !sendDataI2C) || !permissionSendI2C)
    return;

  lastDataI2C = millis();
  sendDataI2C = false;
  
  //Prepare data
  char data[RH_NRF24_MAX_MESSAGE_LEN]; // Buffer big enough for 28-character string
  char result[8]; // Buffer big enough for 7-character float
  char* delim = ":";

  compareData();

  heartbeatOut++;
  if (heartbeatOut >= 100)
    heartbeatOut = 1;

  dtostrf(heartbeatOut, 1, 0, result);
  strcpy(data,result);
  strcat(data,delim);
  dtostrf(lamp.out, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);
  dtostrf(fan.out, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);
  dtostrf(optionData, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);

  mqttClient.loop(); //Give time to WiFi/MQTT to run
  
  // Send a message
  Wire.beginTransmission(I2C_SLAVE_ADDR); // transmit to device with address
  Wire.write(data);
  Wire.endTransmission();    // stop transmitting
  Sprint("Data sent I2C: ");
  Sprintln(data);

  if (millis()-lastDataNRF > 9800) //Insure enough delay between I2C and NRF
    lastDataNRF = 9500;//lastDataNRF = 9797;
}
