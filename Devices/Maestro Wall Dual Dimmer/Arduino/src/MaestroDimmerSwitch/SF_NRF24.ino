// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT NRF24L01+ ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNRF()
{
  if (!radio.begin())
    Sprintln("NRF24 init failed");

  radio.openWritingPipe(pipes[1]); // Read pipe on dimmer
  radio.openReadingPipe(1, pipes[0]); // Write pipe on dimmer
  //RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBM, and RF24_PA_MAX=0dBm
  radio.setPALevel(RF24_PA_LOW);
  //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_1MBPS);

  Sprintln("NRF24 Init Completed");

  lastDataNRF = millis()-8000;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ RUN NRF24L01+ -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void checkNRF()
{
  if (!radio.available())
    return;

  Sprintln("Data received...");

  setBoardLED(true);

  byte buf[RH_NRF24_MAX_MESSAGE_LEN];
  radio.read(&buf, sizeof(buf));
  char * data = (char *)buf;

  Sprintln(data);
  parseData(data, strlen(data));

  setBoardLED(false);
}

//Only send data on value change and after receiving data
void sendNRF()
{
  if (millis() - lastDataNRF < 10000 && !sendDataNRF)
    return;
  
  lastDataNRF = millis();
  //sendDataNRF = false;

  char data[RH_NRF24_MAX_MESSAGE_LEN]; // Buffer big enough for 28-character string
  char result[8]; // Buffer big enough for 7-character float

  uint8_t bufferLvl = 0;
  bufferLvl = lamp.level;
  bitWrite(bufferLvl, STATE_BIT, (lamp.state ? 1 : 0));
  bitWrite(bufferLvl, FADE_BIT, (lamp.fade ? 1 : 0));
  bitWrite(bufferLvl, FULL_BIT, (lamp.full ? 1 : 0));
  lamp.out = bufferLvl;

  bufferLvl = fan.level;
  bitWrite(bufferLvl, STATE_BIT, (fan.state ? 1 : 0));
  bitWrite(bufferLvl, FADE_BIT, (fan.fade ? 1 : 0));
  bitWrite(bufferLvl, FULL_BIT, (fan.full ? 1 : 0));
  fan.out = bufferLvl;

  heartbeatOut++;
  if (heartbeatOut >= 100)
    heartbeatOut = 1;

  uint8_t hb_buffer = heartbeatOut;

  if (buttonPressed || newStart) //Add 100 on button press
  {
    buttonPressed = false;
    hb_buffer += 100;
  }

  dtostrf(hb_buffer, 1, 0, result);
  strcpy(data, result);
  strcat(data, delim);
  //strcat(data,zeroPadding(switchID,6).c_str());
  dtostrf(switchID, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);
  dtostrf(lamp.out, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);
  dtostrf(fan.out, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);
  dtostrf(voltage5V * 100, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);
  dtostrf(DHTTempOut * 10, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);
  dtostrf(DHTHumOut * 10, 1, 0, result);
  strcat(data, result);
  strcat(data, delim);

  // Send a message
  setBoardLED(true);
  
  delay(5);
  radio.stopListening();
  radio.write(&data, sizeof(data));
  Sprint("Data sent: ");
  Sprintln(data);
  delay(5);
  radio.startListening();

  setBoardLED(false);
  
  sendDataNRF = false;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ PARSE FUNCTIONS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
//IMPORTANT - Always send a delimiter at the of string for parsing to succeed
//Expected payload: "HHH:NNNNNN:L:F:D:O:"
//    HHH: is the communication ID
//    NNNNNN: is the switch ID to control
//    L: L is the lamp level 0-7 and embedded state bits for [3..7]
//    F: F is the fan level 0-7 and embedded state bits for [3..7]
//    D: D is the LED level 0-100%
//    O: O is the embedded option bits for [0..7]
void parseData(char* message, int dataSize)
{
  char DELIMITER_DATA = ':';
  int maxDelimimters = 10;
  int maxParsedByte = 8;
  int indexDelim[maxDelimimters]; // ALWAYS END WITH A DELIMITER
  int qtyDelimiters = 0; //Number of delimiters found

  //Find delimiters
  for (int i = 0; i < dataSize; i++)
  {
    if (message[i] == DELIMITER_DATA)
    {
      indexDelim[qtyDelimiters] = i;
      qtyDelimiters++;
      if (qtyDelimiters >= maxDelimimters)
        break;
    }
  }

  uint16_t parsedData[qtyDelimiters]; // ALWAYS END WITH A DELIMITER
  char data[maxParsedByte];

  //Parse between delimiters
  int k = 0;
  int nextLength = 0;
  for (int i = 0; i < dataSize; i++) //Received data array
  {
    if (i == 0)
    {
      nextLength = indexDelim[k];
      i = -1;
    }
    else
      nextLength = indexDelim[k] - indexDelim[k - 1] - 1;

    for (int j = 0; j < nextLength; j++) //character array
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

  //Validate switch ID to start
  uint16_t switchRcvdID = parsedData[1];

  if (switchRcvdID != switchID) //Accept only correct switchID
    return;

  if (parsedData[0] == heartbeatIn) //Don't accept duplicate data
    return;

  heartbeatIn = parsedData[0];

  if (heartbeatIn < 1 || heartbeatIn > 99) //Accept only 1-99 heartbeats
    return;

  //If all conditions met, collect data into variables
  
  Sprint("Parsing: ");
  Sprintln(message);

  lamp.in = parsedData[2];
  lamp.level = parsedData[2];
  lamp.state = bitRead(lamp.level, STATE_BIT) == 1;
  lamp.fade = bitRead(lamp.level, FADE_BIT) == 1;
  lamp.full = bitRead(lamp.level, FULL_BIT) == 1;
  for (int i = 3; i < 8; i++) //3 LSBs used for level 0-7
    bitClear(lamp.level, i);

  fan.in = parsedData[3];
  fan.level = parsedData[3];
  fan.state = bitRead(fan.level, STATE_BIT) == 1;
  fan.fade = bitRead(fan.level, FADE_BIT) == 1;
  fan.full = bitRead(fan.level, FULL_BIT) == 1;
  for (int i = 3; i < 8; i++) //3 LSBs used for level 0-7
    bitClear(fan.level, i);

  float pwmOffset_F = float(parsedData[4])/100.0F;

  if (qtyDelimiters >= 5 && pwmOffset_F != 0) //If offset option data is received
  {
    if (PWM_OFFSET!= pwmOffset_F)
      lastMinute = millis()-10000; //force update EEPROM when value changed
    PWM_OFFSET = constrain(pwmOffset_F, PWM_MIN, PWM_MAX);
  }

  if (qtyDelimiters >= 6) //If option data is received
  {
    optionData = parsedData[5];
    enableBoardLED = bitRead(optionData, LED_BIT) == 1;
  }

  lastDataNRF = 9000;
}

String zeroPadding(int number, int len)
{
  String output = String(number);
  String value;
  int padLen = len - output.length();
  if (padLen <= 0)
    return output;

  value += "0";

  for (int i = 1; i < padLen; i++)
    value += "0";
  value += output;

  return value;
}
