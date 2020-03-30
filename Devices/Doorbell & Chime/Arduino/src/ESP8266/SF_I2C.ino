// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- INIT I2C ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initI2C()
{
  Sprintln("Init I2C...");
  Wire.begin(SDApin, SCLpin);
  outI2C[hrtByte] = 0;
}

void clearI2C()
{
  for (int i=idByte; i<bytesI2C; i++)
    outI2C[i] = 0;
}

void sendI2C()
{
  if (outI2C[hrtByte] == 0)
    outI2C[hrtByte]++;
  
  Wire.beginTransmission(addressI2C);   //Transmit to slave
  for (int i=0; i<bytesI2C; i++)
    Wire.write(outI2C[i]);
  //Wire.write(outI2C, bytesI2C);
  Wire.endTransmission();               //Stop transmitting
  
  outI2C[hrtByte]++;
}

void requestI2C()
{
  int i = 0;
  Wire.requestFrom(9, bytesI2C);
  while (Wire.available())
  {
    inI2C[i] = Wire.read();
    i++;
  }
}
