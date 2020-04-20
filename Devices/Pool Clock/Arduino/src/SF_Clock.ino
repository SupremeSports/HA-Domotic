// ----------------------------------------------------------------------------------------------------
// -------------------------------------- CLOCK DISPLAY FUNCTIONS -------------------------------------
// ----------------------------------------------------------------------------------------------------
void initDisplay()
{
  timeDisplay.Begin(0);   // Start the display with a brightness level of 0%
  tempDisplay.Begin(0);   // Start the display with a brightness level of 0%

  //Wait until the display is initialised before we try to show anything
  while (!timeDisplay.IsReady() || !tempDisplay.IsReady())
  {
    flashBoardLed(50, 1);
    local_delay(10);
  }

  //Init displays values for initial start
  initDisplays = true;
  displayFeature = 96;
  rainbowIndex = 0;
  nextSwitch = millis();
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- TIME DISPLAY FUNCTIONS --------------------------------------
// ----------------------------------------------------------------------------------------------------
//This function gets the text scrolling
void updateTimeString()
{
  //update once per second
  if (Second == prevSecond && !newStart)
    return;
    
  timeDisplay.ForceUppercase(false);
  timeString = setTimeValues();

  Sprintln(timeString);
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- TEMP DISPLAY FUNCTIONS --------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateTempString()
{ 
  //update once per second
  if (Second == prevSecond && !newStart)
    return;

  int moduloSecs = newStart ? 0 : Second % 10;

  if (moduloSecs != 0 && moduloSecs != 4 && moduloSecs != 7)
    prevTempONS = false;

  if (prevTempONS)
    return;

  //Display outside temperature in °C
  if (moduloSecs == 0)
  {
    tempString = set4DigitsValues(outTempC, "C");
    prevTempONS = true;
  }
  //Display outside temperature in °F
  else if (moduloSecs == 4) 
  {
    tempString = set4DigitsValues(outTempF, "F");
    prevTempONS = true;
  }
  //Display outside humidity in %
  else if (moduloSecs == 7) 
  {
    tempString = set4DigitsValues(outHumidity, "h");
    prevTempONS = true;
  }

  Sprintln(tempString);
}

void getOutTemp(String received)
{
  outTempC = received.toFloat();

  if (outTempC != initValue)
    outTempF = celsiusToFahrenheit(outTempC);
  else
    outTempF = float(initValue);
}

void getOutHum(String received)
{
  outHumidity = received.toFloat();
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------ WATER DISPLAY FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateWaterString()
{ 
  //Update once per second
  if (Second == prevSecond && !newStart)
    return;

  int moduloSecs = newStart ? 0 : Second % 10;

  if (moduloSecs != 0 && moduloSecs != 4 && moduloSecs != 7)
    prevWaterONS = false;

  if (prevWaterONS)
    return;

  //Display water temperature in °F
  if (moduloSecs == 0)
  {
    waterString = set4DigitsValues(waterTempF, "F");
    prevWaterONS = true;
  }
  //Display water temperature in °C
  else if (moduloSecs == 4) 
  {
    waterString = set4DigitsValues(waterTempC, "C");
    prevWaterONS = true;
  }
  //Display water pH level
  else if (moduloSecs == 7) 
  {
    waterString = set4DigitsValues(waterLevelPH, "PH");
    prevWaterONS = true;
  }

  Sprintln(waterString);
}

void getWaterTemp(String received)
{
  waterTempF = received.toFloat();
  
  if (waterTempF != initValue)
    waterTempC = fahrenheitToCelsius(waterTempF);
  else
    waterTempC = float(initValue);
}

void getWaterPH(String received)
{
  waterLevelPH = received.toFloat();
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- STRING GENERATION ----------------------------------------
// ----------------------------------------------------------------------------------------------------
//Send the time to the display
String setTimeValues()
{    
  //Set display data
  int len = 2 * timeDigits; //Account for decimal points
  String digitTime[len];
  digitTime[0] = (Hour<10) ? ((Hour==0)?"0":" ") : String(Hour/10);
  digitTime[2] = String(Hour%10);
  digitTime[4] = (Minute<10) ? "0" : String(Minute/10);
  digitTime[6] = String(Minute%10);
  digitTime[8] = (Second<10) ? "0" : String(Second/10);
  digitTime[10] = String(Second%10);

  digitTime[1] = ((Second % 2)==0) ? "." : "";
  digitTime[3] = ((Second % 2)==0) ? "." : "";
  digitTime[5] = ((Second % 2)==0) ? "." : "";
  digitTime[7] = ((Second % 2)==0) ? "." : "";
  digitTime[9] = "";  //Always off on time display
  digitTime[11] = ""; //Always off on time display

  String stringOutput;
  for (int i=0; i<len; i++)
    stringOutput += digitTime[i];

  return stringOutput;
}

//Array for displaying digits
String set4DigitsValues(float value, String units)
{
  String stringOutput = "";
  bool decimalPoint = false;
  //If network connection never initialized, display dashes
  if ((!networkActive && !localTimeValid && newStart) || value == initValue)
    stringOutput = "----" + units;
  else
  {
    int newValue;
    if (value > 99.9 || value < -9.9)
    {
      newValue = abs(value);

      //Round up if decimal >= 5
      int decimal = int(abs(value*10)) % 10;
      if (decimal >= 5)
        newValue += 1;
    }
    else
      newValue = abs(value*10);

    //Set display data
    int len = tempDigits; //Account for decimal points
    String digitValue[len];
    if (value < 10)
    {
      digitValue[0] = (value < 0) ? "-" : " ";
      digitValue[2] = String(newValue/10);
    }
    else
    {
      digitValue[0] = String(newValue/100);
      digitValue[2] = String((newValue%100)/10);
    }
      
    digitValue[4] = String(newValue%10);
    digitValue[6] = units;
  
    //Set decimal point if necessary
    digitValue[1] = ""; //Never used
    digitValue[5] = ""; //Never used
    digitValue[7] = ""; //Never used
  
    //Decimal point
    if (value > 99.9 || value < -9.9)
    {
      digitValue[3] = "";
      decimalPoint = false;
    }
    else
    {
      digitValue[3] = ".";
      decimalPoint = true;
    }

    for (int i=0; i<len; i++)
      stringOutput += digitValue[i];
  }
  
  int outLen = stringOutput.length();
  outLen -= decimalPoint ? 1 : 0; //remove decimal point from length
  if (outLen > 4)
    stringOutput.remove(0, outLen-4);

  return stringOutput;
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- TEXT DISPLAY FUNCTIONS -------------------------------------
// ----------------------------------------------------------------------------------------------------
//This function gets the text scrolling
void updateTimeText()
{
  if ((millis()-timeTextScrollLast) < configScrollSpeed || newStart)
    return;

  timeTextScrollLast = millis();

  timeTextString = timeTextStringBuffer;
  int len = timeTextString.length() - timeDigits;
  
  if (len <= 0)
    return;
  
  if (len < timeDigits)
    rollingTimeTextIndex = 0;

  timeTextString = timeTextString.substring(rollingTimeTextIndex, rollingTimeTextIndex+timeDigits);

  Sprint("TextTime: ");
  Sprintln(timeTextString);

  rollingTimeTextIndex++;
  if (rollingTimeTextIndex >= len)
    rollingTimeTextIndex = 0;
}

//This function gets the text scrolling
void updateTempText()
{
  if ((millis()-tempTextScrollLast) < configScrollSpeed || newStart)
    return;

  tempTextScrollLast = millis();

  tempTextString = tempTextStringBuffer;
  int len = tempTextString.length() - tempDigits;

  if (len <= 0)
    return;

  if (len < tempDigits)
    rollingTempTextIndex = 0;

  tempTextString = tempTextString.substring(rollingTempTextIndex, rollingTempTextIndex+tempDigits);

  Sprint("TextTemp: ");
  Sprintln(tempTextString);

  rollingTempTextIndex++;
  if (rollingTempTextIndex >= len)
    rollingTempTextIndex = 0;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- STRING PADDING FUNCTIONS -------------------------------------
// ----------------------------------------------------------------------------------------------------
//Set string from PLC and fill with blank spaces
String padString(String received, uint8_t digits)
{
  String data = "";

  if (received == "null" || received == "")
    return data;
    
  data += adjustString(received);
  int len = data.length();

  //If string shorter than number of digits
  //Add trailing blanks
  if (len <= digits)
  {
    for (int i=len; i<digits; i++)
      data += " ";
      
    return data;
  }

  //If string longer than number of digits
  String rollingText = "";
  
  //Add leading blanks
  for (int i=0; i<digits; i++)
    rollingText += " ";

  rollingText += data; //Add text string

  //Add trailing blanks
  for (int i=0; i<digits; i++)
    rollingText += " ";

  return rollingText;
}

//Force uppercase characters
//Replace letters that cannot be reproduced on 7-segment
//read character c:
//if not acceptable, replace with a space
String adjustString(String str)
{
  //str.toUpperCase();
  str.replace("K"," ");
  str.replace("k"," ");
  str.replace("M"," ");
  str.replace("m"," ");
  str.replace("V"," ");
  str.replace("v"," ");
  str.replace("W"," ");
  str.replace("w"," ");

  String output = "";
  for(int i=0; i<str.length(); i++)
  {
    char test = str.charAt(i);
    if (isAlphaNumeric(test) || isWhitespace(test) || str.charAt(i) == '.' || str.charAt(i) == '-' || str.charAt(i) == '_')
      output += test;
    else
      output += " ";
  }

  return output;
}

//read character c:
//if c is - or . (decimal point): get next character
//  Only one of each is accepted
//if c is digit: get next character
//if c is something else: stop it is not a number
boolean isValidNumber(String str)
{
  int dotCount = 0;
  int dashCount = 0;
  bool isNum = false;
  for(int i=0; i<str.length(); i++)
  {
    isNum = isDigit(str.charAt(i)) || str.charAt(i) == '.' || str.charAt(i) == '-';

    if (str.charAt(i) == '.')
      dotCount++;
    else if (str.charAt(i) == '-')
      dashCount++;
      
    if (!isNum || dotCount>1 || dashCount>1)
      return false;
  }
  
  return isNum;
}
