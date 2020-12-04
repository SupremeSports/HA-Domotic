// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- I2C SETUP ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initI2C()
{
  Wire.begin(I2C_SLAVE_ADDR);         //Join i2c bus with address #4
  Wire.onReceive(receiveEvent);       //Register event

  #ifdef ENABLE_IRQ
    pinMode(I2C_IRQ, INPUT);
  #endif

  wdtReset();

  Sprintln("I2C Init Completed!");
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- I2C RUN ----------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Save data for later
// This is an interrupt routine and may delay dimmer functions
// So to prevent flickering, lets save data and parse it when we have spare time
void receiveEvent(int howMany)
{
  setBoardLED(HIGH);

  char DELIMITER_DATA = ':';
  char msgBuf[MAX_I2C_LENGTH];
  int i = 0;
  int j = 0;
  
  while (Wire.available() && i<MAX_I2C_LENGTH) //Loop through all bytes
  {  
    msgBuf[i] = Wire.read(); //Receive byte as a character
    i++;
    
    if (strcmp(msgBuf[i], DELIMITER_DATA)==0)
      j = i;
  }

  //Filter out wrong/erroneous I2C data
  // Data must end with a delimiter, if not trunc out everything after last delimiter
  messageLength = j;
  for (int k=0; k<j; k++)
    message[k] = msgBuf[k];

  // Data must end with a delimiter, if not discard data by putitng zero length
  if (message[j-1] != DELIMITER_DATA)
    messageLength = 0;
}

//Run data parsing whenever we have spare time
void runI2C()
{
  wdtReset();

  checkStatusI2C();
  
  if (messageLength == 0)
    return;

  lastDataI2C = millis(); //Reset timer

  parseDataI2C(message, messageLength);
  messageLength = 0;
  
  setBoardLED(LOW);
}

void checkStatusI2C()
{
  #ifdef USE_SELFRESET
    if (millis()-lastDataI2C > maxDataI2CDelay)
      software_Reset();
  #endif
  
  if (millis()-lastDataI2C > maxDataI2CDelay)
  {
    Sprintln("I2C Issues...");
    TWCR = 0;
    initI2C();

    lastDataI2C = millis();
  }
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ PARSE FUNCTIONS -----------------------------------------
// ----------------------------------------------------------------------------------------------------
//IMPORTANT - Always send a delimiter at the end of string for parsing to succeed
// ALWAYS END WITH A DELIMITER
//Expected payload: "HHH:L:F:O:"
//    HHH: is the communication ID (Heartbeat)
//    L: L is the lamp level 0-7 and embedded state bits for [3..7]
//    F: F is the fan level 0-7 and embedded state bits for [3..7]
//    O: O is the embedded option bits for [0..7]
void parseDataI2C(char* message, int dataSize)
{
  wdtReset();

  char DELIMITER_DATA = ':';
  int maxDelimimters = 10;
  int maxParsedByte = 8;
  int indexDelim[maxDelimimters]; // ALWAYS END WITH A DELIMITER
  int qtyDelimiters = 0; //Number of delimiters found

  Sprint("Parsing: ");
  //Find delimiters
  for (int i=0; i<dataSize; i++)
  {
    Sprint(message[i]);
    if (message[i] == DELIMITER_DATA)
    {
      indexDelim[qtyDelimiters] = i;
      qtyDelimiters++;
      if (qtyDelimiters>=maxDelimimters)
        break;
    }
  }
  Sprintln();

  if (qtyDelimiters < 3) //If not enough delimiters found
    return;
  
  uint16_t parsedData[qtyDelimiters]; // ALWAYS END WITH A DELIMITER
  char data[maxParsedByte];

  //Parse between delimiters
  int k = 0;
  int nextLength = 0;
  for (int i=0; i<dataSize; i++)    //Received data array
  {
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

  if (parsedData[0] == heartbeatIn) //Don't accept duplicate data
    return;

  heartbeatIn = parsedData[0];

  if (heartbeatIn<1 || heartbeatIn>99) //Accept only 1-99 heartbeats
    return;
  
  //*******************************************************
  //If all conditions met, collect data into variables
  //*******************************************************

  //LAMP data to extract
  lamp.level = parsedData[1];
  lamp.state = bitRead(lamp.level, STATE_BIT)==1;
  lamp.fade = bitRead(lamp.level, FADE_BIT)==1;
  lamp.full = bitRead(lamp.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(lamp.level, i);

  //FAN data to extract
  fan.level = parsedData[2];
  fan.state = bitRead(fan.level, STATE_BIT)==1;
  fan.fade = bitRead(fan.level, FADE_BIT)==1;
  fan.full = bitRead(fan.level, FULL_BIT)==1;
  for (int i=3; i<8; i++) //3 LSBs used for level 0-7
    bitClear(fan.level, i);
  
  if (qtyDelimiters >= 4) //If option data is received
  {
    optionData = parsedData[3];
    enableBoardLED = bitRead(optionData, LED_BIT)==1;
  }
}
