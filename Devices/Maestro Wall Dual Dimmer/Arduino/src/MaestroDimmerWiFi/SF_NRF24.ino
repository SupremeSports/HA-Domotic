// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT NRF24L01+ ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNRF()
{
  if (!nrf24.init())
    Sprintln("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(nrfChannel))
    Sprintln("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Sprintln("setRF failed");

  local_delay(100);
  Sprintln("NRF24 Init Completed");

  compareData();
  sendNRF();
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ RUN NRF24L01+ -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkNRF()
{
  if (!nrf24.available())
    return;
    
  //Sprintln("Data received...");
  setBoardLED(false);
  
  byte buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  nrf24.recv(buf, &len);

  setBoardLED(false);

  mqttClient.loop(); //Give time to WiFi/MQTT to run

  if (newDataMQTT || updateMQTT || sendDataNRF)
    return;

  char * dataOut = (char *)buf;
  parseData(dataOut, strlen(dataOut));
}

void sendNRF()
{
  //if ((millis()-lastDataNRF < 10000 && !sendDataNRF) || !permissionSendI2C)
  //  return;

  if (!sendDataNRF || !permissionSendI2C)
    return;

  lastDataNRF = millis();
  sendDataNRF = false;

  //Prepare data
  char data[RH_NRF24_MAX_MESSAGE_LEN]; // Buffer big enough for 28-character string
  char result[8]; // Buffer big enough for 7-character float
  char* delim = ":";

  compareData();

  heartbeatOut++;
  if (heartbeatOut >= 100)
    heartbeatOut = 1;

  mqttClient.loop(); //Give time to WiFi/MQTT to run

  dtostrf(heartbeatOut, 1, 0, result);
  strcpy(data,result);
  strcat(data,delim);
  //strcat(data,zeroPadding(switchID,6).c_str());
  dtostrf(switchID, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);
  dtostrf(lamp.out, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);
  dtostrf(fan.out, 1, 0, result);
  mqttClient.loop(); //Give time to WiFi/MQTT to run
  strcat(data,result);
  strcat(data,delim);
  dtostrf(PWM_OFFSET, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);
  dtostrf(optionData, 1, 0, result);
  strcat(data,result);
  strcat(data,delim);

  // Send a message
  uint8_t dataOutNRF[sizeof(data)];
  for (int i=0; i<sizeof(data); i++)
    dataOutNRF[i] = data[i];
  
  for (int i=0; i<1; i++)
  {
    mqttClient.loop(); //Give time to WiFi/MQTT to run
    nrf24.send(dataOutNRF, sizeof(dataOutNRF));
    local_delay(100);
  }

  while(!nrf24.waitPacketSent()){}

  Sprint("Data sent NRF: ");
  Sprintln(data);
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ PARSE FUNCTIONS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
//IMPORTANT - Always send a delimiter at the of string for parsing to succeed
//Expected payload: "HHH:NNNNNN:L:F:VVV:"
//    HHH: is the communication ID
//    NNNNNN: is the switch ID to control
//    L: L is the lamp level 0-7 and embedded state bits for [3..7]
//    F: F is the fan level 0-7 and embedded state bits for [3..7]
//    VVV: is the switch 5V bus voltage
void parseData(char* message, int dataSize)
{
  char DELIMITER_DATA = ':';
  int maxDelimimters = 10;
  int maxParsedByte = 8;
  int indexDelim[maxDelimimters]; // ALWAYS END WITH A DELIMITER
  int qtyDelimiters = 0; //Number of delimiters found

  //Find delimiters
  for (int i=0; i<dataSize; i++)
  {
    if (message[i] == DELIMITER_DATA)
    {
      indexDelim[qtyDelimiters] = i;
      qtyDelimiters++;
      if (qtyDelimiters>=maxDelimimters)
        break;
    }
  }
  
  uint16_t parsedData[qtyDelimiters]; // ALWAYS END WITH A DELIMITER
  char data[maxParsedByte];

  //Parse between delimiters
  int k = 0;
  int nextLength = 0;
  for (int i=0; i<dataSize; i++)    //Received data array
  {
    mqttClient.loop(); //Give time to WiFi/MQTT to run
    if (i==0)
    {
      nextLength = indexDelim[k];
      i = -1;
    }
    else
      nextLength = indexDelim[k] - indexDelim[k-1]-1;

    for (int j=0; j<nextLength; j++)   //character array
    {
      i++; //Skip delimiter and/or go to next character
      data[j] = message[i];
    }
    data[nextLength] = '\0';
    parsedData[k] = atoi(data);
    k++; //Next parsing
    if (k >= qtyDelimiters)
      break;
  }

  //NOW, EXTRACT DATA

  mqttClient.loop(); //Give time to WiFi/MQTT to run

  //Validate switch ID to start
  switchRcvdID = parsedData[1];

  if (switchRcvdID != switchID) //Accept only correct switchID
    return;

  if (parsedData[0] == heartbeatIn) //Reject duplicate data
    return;

  heartbeatIn = parsedData[0];

  permissionSendNRF = false;
  if (heartbeatIn>101 && heartbeatIn<199) //101-199 means a button is pressed
  {
    updateNRF = true;
    permissionSendNRF = true;
  }
  else if (heartbeatIn<1 || heartbeatIn>99) //Accept only 1-99 heartbeats
    return;

  //If all conditions met, collect data into variables

  Sprint("Parsing: ");
  Sprintln(message);
  
  lampNRF.level = parsedData[2];
  lampNRF.state = bitRead(lampNRF.level, STATE_BIT)==1;
  lampNRF.full = bitRead(lampNRF.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(lampNRF.level, i);
    
  fanNRF.level = parsedData[3];
  fanNRF.state = bitRead(fanNRF.level, STATE_BIT)==1;
  fanNRF.full = bitRead(fanNRF.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(fanNRF.level, i);

  voltage5VSwitch = parsedData[4]/100.0F;

  newDataNRF = true;
  permissionSendI2C = true;
  compareData();
  lastDataI2C = millis()-10000;

  if (millis()-lastDataNRF > 9800) //Insure enough delay between I2C and NRF
    lastDataNRF = 9797;

  mqttClient.loop(); //Give time to WiFi/MQTT to run
}

String zeroPadding(int number, int len)
{
  String output = String(number);
  String value;
  int padLen = len - output.length();
  if (padLen<=0)
    return output;

  value += "0";
  
  for (int i=1; i<padLen; i++)
    value += "0";
  value += output;

  return value;
}
