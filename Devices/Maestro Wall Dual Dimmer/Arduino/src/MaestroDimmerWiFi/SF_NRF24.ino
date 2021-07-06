// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT NRF24L01+ ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNRF()
{
  if (!radio.begin())
    Sprintln("NRF24 init failed");

  radio.openWritingPipe(pipes[0]); // Read pipe on switch
  radio.openReadingPipe(1, pipes[1]); // Write pipe on switch
  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
  radio.setPALevel(RF24_PA_HIGH);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_1MBPS);
  
  Sprintln("NRF24 Init Completed");

  compareData();
  radio.startListening();
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ RUN NRF24L01+ -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkNRF()
{
  if (!radio.available())
  {
    if (newDataMQTT || updateMQTT || sendDataNRF)
      sendNRF();
    return;
  }

  setBoardLED(true);
  
  byte buf[RH_NRF24_MAX_MESSAGE_LEN];
  radio.read(&buf, sizeof(buf));
  char * data = (char *)buf;

  setBoardLED(false);

  mqttClient.loop(); //Give time to WiFi/MQTT to run

  if (newDataMQTT || updateMQTT || sendDataNRF)
  {
    sendNRF();
    return;
  }

  Sprintln("Data received...");
  Sprintln(data);
  parseData(data, strlen(data));  

  mqttClient.loop(); //Give time to WiFi/MQTT to run
}

void sendNRF()
{
  if (!sendDataNRF || !permissionSendI2C)
  {
    sendDataNRF = false;
    return;
  }
  
  sendDataNRF = false;

  //Prepare data
  char data[RH_NRF24_MAX_MESSAGE_LEN]; // Buffer big enough for 28-character string
  char result[8]; // Buffer big enough for 7-character float

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
  delay(5);
  radio.stopListening();
  radio.write(&data, sizeof(data));
  Sprint("Data sent: ");
  Sprintln(data);
  delay(5);
  radio.startListening();

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
//    TTT: is the switch room temperature
//    HHH: is the switch room humidity
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
  uint16_t switchRcvdID = parsedData[1];

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
  lampNRF.fade = bitRead(lampNRF.level, FADE_BIT) == 1;
  lampNRF.full = bitRead(lampNRF.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(lampNRF.level, i);
    
  fanNRF.level = parsedData[3];
  fanNRF.state = bitRead(fanNRF.level, STATE_BIT)==1;
  fanNRF.fade = bitRead(fanNRF.level, FADE_BIT) == 1;
  fanNRF.full = bitRead(fanNRF.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(fanNRF.level, i);

  voltage5VSwitch = (parsedData[4]/100.0F)*1.25F; //Unknown voltage discrepancy to verify

  DHTTempOut =  (parsedData[5]/10.0F)-5.0F; //Unknown temperature discrepancy to verify
  DHTHumOut =  parsedData[6]/10.0F;

  newDataNRF = true;
  permissionSendI2C = true;
  compareData();
  lastDataI2C = millis()-10000;

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
