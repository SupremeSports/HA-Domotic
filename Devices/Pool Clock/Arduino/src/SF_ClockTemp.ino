// ----------------------------------------------------------------------------------------------------
// ------------------------------------ STRING DISPLAY FUNCTIONS --------------------------------------
// ----------------------------------------------------------------------------------------------------
//This function gets the text scrolling
void updateTimeText()
{
  if ((millis()-timeTextScrollLast) < timeTextScrollSpeed || newStart)
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
  if ((millis()-tempTextScrollLast) < tempTextScrollSpeed || newStart)
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

//Set string from PLC and fill with blank spaces
String padString(String received, uint8_t digits)
{
  String data = "";

  if (received == "null")
    return data;
    
  data += received;
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

// ----------------------------------------------------------------------------------------------------
// ------------------------------------- TEMP DISPLAY FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateTemp()
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
  outTempF = celsiusToFahrenheit(outTempC);
}

void getOutHum(String received)
{
  outHumidity = received.toFloat();
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------ WATER DISPLAY FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateWater()
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
  waterTempC = fahrenheitToCelsius(waterTempF);
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
  int len = 2 * timeDigits;
  //Set displays data
  String digitTime[len];
  digitTime[0] = (Hour<10) ? " " : String(Hour/10);
  digitTime[2] = String(Hour%10);
  digitTime[4] = (Minute<10) ? "0" : String(Minute/10);
  digitTime[6] = String(Minute%10);
  digitTime[8] = (Second<10) ? "0" : String(Second/10);
  digitTime[10] = String(Second%10);

  digitTime[1] = ((Second % 2)==0) ? "." : "";
  digitTime[3] = ((Second % 2)==0) ? "." : "";
  digitTime[5] = ((Second % 2)==0) ? "." : "";
  digitTime[7] = ((Second % 2)==0) ? "." : "";
  digitTime[9] = ((Second % 2)==0) ? "" : "";  //Always off on time display
  digitTime[11] = ((Second % 2)==0) ? "" : ""; //Always off on time display

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
  
    //Set displays data
    String digitValue[8];
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
    digitValue[1] = "";
    digitValue[5] = "";
    digitValue[7] = "";
  
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

    for (int i=0; i<8; i++)
      stringOutput += digitValue[i];
  }
  
  int len = stringOutput.length();
  len -= decimalPoint ? 1 : 0; //remove decimal point from length
  if (len > 4)
    stringOutput.remove(0, len-4);

  return stringOutput;
}
