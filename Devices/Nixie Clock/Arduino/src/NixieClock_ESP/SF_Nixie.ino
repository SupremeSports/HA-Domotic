// ----------------------------------------------------------------------------------------------------
// -------------------------------------------- INIT NIXIE --------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNixie()
{
  Sprintln("Init Nixie...");

  getClockOptionsFromI2C();
  local_delay(50);                  //Wait for all data to be ready
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- INIT I2C ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initI2C()
{
  Sprintln("Init I2C...");
  Wire.begin(I2C_SDA, I2C_SCL);

  // Find I2C slave device address
  while (!scanI2CBus())
    flashBoardLed(250, 2);

  local_delay(50);                  //Wait for all data to be ready

  // Allow the IP to be displayed on the clock
  sendIPAddressToI2C(WiFi.localIP());
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- I2C functions ------------------------------------------
// ----------------------------------------------------------------------------------------------------
// Send the time to the I2C slave. If the transmission went OK, return true, otherwise false.
boolean sendTimeToDisplay()
{
  byte Year_byte = (Year - 2000); //Maximum of a byte is 256, so subtract 2000 in order to pass it properly
  byte Month_byte = Month;
  byte day_byte = Day;
  byte Hour_byte = Hour;
  byte Minute_byte = Minute;
  byte Second_byte = Second;

  Wire.beginTransmission(preferredI2CSlaveAddress);
  Wire.write(I2C_TIME_UPDATE); // Command
  Wire.write(Year_byte);
  Wire.write(Month_byte);
  Wire.write(day_byte);
  Wire.write(Hour_byte);
  Wire.write(Minute_byte);
  Wire.write(Second_byte);
  
  int error = Wire.endTransmission();
  return (error == 0);
}

void sendDataToI2C(uint8_t command, uint16_t data)
{
  //Config data for I2C communication
  bool configBool = data==1 ? true : false;;
  byte configBytes = byte(data);
  uint16_t configInt = data;

  switch(command)
  {
    case I2C_SET_OPTION_12_24:
      setClockOption12H24H(configBool);
      break;
    case I2C_SET_OPTION_BLANK_LEAD:
      setClockOptionBlankLeadingZero(configBool);
      break;
    case I2C_SET_OPTION_SCROLLBACK:
      setClockOptionScrollback(configBool);
      break;
    case I2C_SET_OPTION_SUPPRESS_ACP:
      setClockOptionSuppressACP(configBool);
      break;
    case I2C_SET_OPTION_FADE:
      setClockOptionUseFade(configBool);
      break;
    case I2C_SET_OPTION_USE_LDR:
      setClockOptionUseLDR(configBool);
      break;
    case I2C_SET_OPTION_SLOTS_MODE:
      setClockOptionSlotsMode(configBytes);
      break;
    case I2C_SET_OPTION_DATE_FORMAT:
      setClockOptionDateFormat(configBytes);
      break;
    case I2C_SET_OPTION_DAY_BLANKING:
      setClockOptionDayBlanking(configBytes);
      break;
    case I2C_SET_OPTION_BLANK_START:
      setClockOptionBlankFrom(configBytes);
      break;
    case I2C_SET_OPTION_BLANK_END:
      setClockOptionBlankTo(configBytes);
      break;
    case I2C_SET_OPTION_FADE_STEPS:
      setClockOptionFadeSteps(configBytes);
      break;
    case I2C_SET_OPTION_SCROLL_STEPS:
      setClockOptionScrollSteps(configBytes);
      break;
    case I2C_SET_OPTION_BACKLIGHT_MODE:
      setClockOptionBacklightMode(configBytes);
      break;
    case I2C_SET_OPTION_RED_CHANNEL:
      setClockOptionRedCnl(configBytes);
      break;
    case I2C_SET_OPTION_GREEN_CHANNEL:
      setClockOptionGrnCnl(configBytes);
      break;
    case I2C_SET_OPTION_BLUE_CHANNEL:
      setClockOptionBluCnl(configBytes);
      break;
    case I2C_SET_OPTION_CYCLE_SPEED:
      setClockOptionCycleSpeed(configBytes);
      break;
    case I2C_SET_OPTION_BLANK_MODE:
      setClockOptionBlankMode(configBytes);
      break;
    case I2C_SET_OPTION_MIN_DIM:
      setClockOptionMinDim(configInt);
      break;
    default:
      break;
  }
}

boolean setClockOption12H24H(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_12_24, newMode);
}

boolean setClockOptionBlankLeadingZero(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_BLANK_LEAD, newMode);
}

boolean setClockOptionScrollback(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_SCROLLBACK, newMode);
}

boolean setClockOptionSuppressACP(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_SUPPRESS_ACP, newMode);
}

boolean setClockOptionUseFade(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_FADE, newMode);
}

boolean setClockOptionUseLDR(boolean newMode)
{
  return setClockOptionBoolean(I2C_SET_OPTION_USE_LDR, newMode);
}

boolean setClockOptionDateFormat(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_DATE_FORMAT, newMode);
}

boolean setClockOptionDayBlanking(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_DAY_BLANKING, newMode);
}

boolean setClockOptionBlankFrom(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_BLANK_START, newMode);
}

boolean setClockOptionBlankTo(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_BLANK_END, newMode);
}

boolean setClockOptionFadeSteps(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_FADE_STEPS, newMode);
}

boolean setClockOptionScrollSteps(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_SCROLL_STEPS, newMode);
}

boolean setClockOptionBacklightMode(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_BACKLIGHT_MODE, newMode);
}

boolean setClockOptionRedCnl(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_RED_CHANNEL, newMode);
}

boolean setClockOptionGrnCnl(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_GREEN_CHANNEL, newMode);
}

boolean setClockOptionBluCnl(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_BLUE_CHANNEL, newMode);
}

boolean setClockOptionCycleSpeed(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_CYCLE_SPEED, newMode);
}

boolean setClockOptionBlankMode(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_BLANK_MODE, newMode);
}

boolean setClockOptionSlotsMode(byte newMode)
{
  return setClockOptionByte(I2C_SET_OPTION_SLOTS_MODE, newMode);
}

boolean setClockOptionMinDim(unsigned int newMinDim)
{
  return setClockOptionInt(I2C_SET_OPTION_MIN_DIM, newMinDim);
}

//Get the options from the I2C slave. If the transmission went OK, return true, otherwise false
boolean getClockOptionsFromI2C()
{ 
  int available = Wire.requestFrom((int)preferredI2CSlaveAddress, I2C_DATA_SIZE);
  
  if (available == I2C_DATA_SIZE)
  {
    // IDX - Got protocol header
    byte receivedByte = Wire.read();

    // IDX - I2C Protocol ERROR! Expected header
    if (receivedByte != I2C_PROTOCOL_NUMBER)
      return false;
    
    // IDX 1 - Got hour mode
    receivedByte = Wire.read();
    configHourMode = receivedByte;

    // IDX 2 - Got blank lead
    receivedByte = Wire.read();
    configBlankLead = receivedByte;

    // IDX 3 - Got scrollback
    receivedByte = Wire.read();
    configScrollback = receivedByte;

    // IDX 4 - Got suppress ACP
    receivedByte = Wire.read();
    configSuppressACP = receivedByte;

    // IDX 5 - Got useFade
    receivedByte = Wire.read();
    configUseFade = receivedByte;

    // IDX 6 - Got date format
    receivedByte = Wire.read();
    configDateFormat = receivedByte;

    // IDX 7 - Got day blanking
    receivedByte = Wire.read();
    configDayBlanking = receivedByte;

    // IDX 8 - Got blank hour start
    receivedByte = Wire.read();
    configBlankFrom = receivedByte;

    // IDX 9 - Got blank hour end
    receivedByte = Wire.read();
    configBlankTo = receivedByte;

    // IDX 10 - Got fade steps
    receivedByte = Wire.read();
    configFadeSteps = receivedByte;

    // IDX 11 - Got scroll steps
    receivedByte = Wire.read();
    configScrollSteps = receivedByte;

    // IDX 12 - Got backlight mode
    receivedByte = Wire.read();
    configBacklightMode = receivedByte;

    // IDX 13 - Got red channel
    receivedByte = Wire.read();
    configRedCnl = receivedByte;

    // IDX 14 - Got green channel
    receivedByte = Wire.read();
    configGreenCnl = receivedByte;

    // IDX 15 - Got blue channel
    receivedByte = Wire.read();
    configBlueCnl = receivedByte;

    // IDX 16 - Got cycle speed
    receivedByte = Wire.read();
    configCycleSpeed = receivedByte;

    // IDX 17 - Got useLDR
    receivedByte = Wire.read();
    configUseLDR = receivedByte;

    // IDX 18 - Got blankMode
    receivedByte = Wire.read();
    configBlankMode = receivedByte;

    // IDX 19 - Got slotsMode
    receivedByte = Wire.read();
    configSlotsMode = receivedByte;
    
    // IDX 20 - Got minDim Hi
    receivedByte = Wire.read();
    int newMinDim = receivedByte * 256;
    
    // IDX 21 - Got minDim Lo 
    receivedByte = Wire.read();
    newMinDim += receivedByte;
    
    // Get minDim combined
    configMinDim = newMinDim;
  }
  //else, didn't get the right number of bytes
  
  int error = Wire.endTransmission();

  if (error == 0)
    getOnOffStates();

  return (error == 0);
}

//Sending IP Address to I2C
boolean sendIPAddressToI2C(IPAddress ip)
{
  Wire.beginTransmission(preferredI2CSlaveAddress);
  Wire.write(I2C_SHOW_IP_ADDR); // Command
  Wire.write(ip[0]);
  Wire.write(ip[1]);
  Wire.write(ip[2]);
  Wire.write(ip[3]);
  int error = Wire.endTransmission();

  return (error == 0);
}

//Send the options from the I2C slave. If the transmission went OK, return true, otherwise false.
boolean setClockOptionBoolean(byte option, boolean newMode)
{
  Wire.beginTransmission(preferredI2CSlaveAddress);
  Wire.write(option);
  byte newOption;
  newOption = newMode ? 0 : 1;
  Wire.write(newOption);
  int error = Wire.endTransmission();

  return (error == 0);
}

//Send the options from the I2C slave. If the transmission went OK, return true, otherwise false.
boolean setClockOptionByte(byte option, byte newMode)
{
  Wire.beginTransmission(preferredI2CSlaveAddress);
  Wire.write(option);
  Wire.write(newMode);
  int error = Wire.endTransmission();

  return (error == 0);
}

//Send the options from the I2C slave. If the transmission went OK, return true, otherwise false.
boolean setClockOptionInt(byte option, int newMode)
{
  byte loByte = newMode % 256;
  byte hiByte = newMode / 256; 
  
  Wire.beginTransmission(preferredI2CSlaveAddress);
  Wire.write(option);
  Wire.write(hiByte);
  Wire.write(loByte);
  int error = Wire.endTransmission();

  return (error == 0);
}

//Scanning I2C bus to find address
boolean scanI2CBus()
{
  byte pingAnsweredFrom = 0xff;

  //Check prefered I2C address
  Wire.beginTransmission(preferredI2CSlaveAddress);
  int error1 = Wire.endTransmission();
  if (error1 == 0)
  {
    preferredI2CSlaveAddress = 0x69;
    preferredAddressFoundBy = 1;
    if (getClockOptionsFromI2C()) 
      pingAnsweredFrom = preferredI2CSlaveAddress;
  }

  // if we got a ping answer, then we must use that
  if (pingAnsweredFrom != 0xff)
  {
    preferredI2CSlaveAddress = pingAnsweredFrom;
    preferredAddressFoundBy = 2;
    return true;
  }

  return false;
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- TEMP I2C ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
float getI2CTemp()
{
  return(int(getI2CTempReg(temp_reg)) / 256.0F);
}

unsigned getI2CTempReg(byte reg)
{
  unsigned Result=0xFFFF;

  Sprint("getReg "); 
  Sprintln(reg);

  Wire.beginTransmission(preferredI2CTempAddress);
  Wire.write(reg); // pointer reg
  Wire.endTransmission();

  uint8_t c;

  Wire.requestFrom(preferredI2CTempAddress, uint8_t(2));
  if(Wire.available())
  {
    c = Wire.read();
    Result = c;
    if(reg != config_reg)
    {
      Result = (unsigned(c))<<8;
      if(Wire.available())
      {
          c = Wire.read();
          Result |= (unsigned(c));
      }
      else
        Result = 0xFFFF;
    }
  }

  Wire.endTransmission();

  return(Result);
}
