// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- I2C SETUP --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initI2C()
{
  Sprintln("Init I2C...");
  Wire.begin(addressI2C); 
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  for (int i=0; i<bytesI2C; i++)
  {
    dataIn[i] = 0;
    dataOut[i] = 0;
  }
}

//When a new request arrives on I2C, check which action is needed
void runI2C()
{
  if (!newI2Crequest)
    return;
    
  newI2Crequest = false;

  soundRequest();

  //Clear input data but not heartbeat
  for (int i=idByte; i<bytesI2C; i++)
    dataIn[i] = 0;

  //Display buffer values after cleared
  for (int i=0; i<bytesI2C; i++)
  {
    Sprint(dataIn[i]);
    if (i < bytesI2C-1)
      Sprint(", ");
    else
      Sprintln("");
  }
}

void receiveEvent(int bytes)
{
  Sprintln("Data received...");
  for (int i=0; i<bytesI2C; i++)
  {
    dataIn[i] = Wire.read();
    delay(5);
  }

  dataOut[hrtByte] = dataIn[hrtByte];   //Heartbeat

  //Display buffer values after received
  for (int i=0; i<bytesI2C; i++)
  {
    Sprint(dataIn[i]);
    if (i < bytesI2C-1)
      Sprint(", ");
    else
      Sprintln("");
  }

  if (dataIn[idByte] != 0)
    newI2Crequest = true;
}

void requestEvent()
{
  for (int i=0; i<bytesI2C; i++)
    Wire.write(dataOut[i]);
    
  Sprintln("Data requested...");
}
