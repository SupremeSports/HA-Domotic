// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- TIME FUNCTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//IMPORTANT - Always send a delimiter at the of string for parsing to succeed
//Expected payload: "YYYY:MM:DD:hh:mm:ss:"
void parseTime(String data)
{
  int dataSize = 6;
  int indexDelim[dataSize]; // ALWAYS END WITH A DELIMITER

  indexDelim[0] = data.indexOf(DELIMITER_TIME);
  for (int i = 1; i<dataSize-1; i++)
    indexDelim[i] = data.indexOf(DELIMITER_TIME, indexDelim[i-1]+1);
  indexDelim[dataSize-1] = data.lastIndexOf(DELIMITER_TIME);

  uint16_t dateTime[dataSize];
  dateTime[0] = data.substring(0, indexDelim[0]).toInt();
  for (int i = 1; i<dataSize; i++)
    dateTime[i] = data.substring(indexDelim[i-1]+1, indexDelim[i]).toInt();

  setTime(dateTime[3], dateTime[4], dateTime[5], dateTime[2], dateTime[1], dateTime[0]);

  Second = 100;
  updateTime();

  localTimeValid = (Year>2000);
}

void updateTime()
{
  if (second() == Second)
    return;
  
  Year = year();
  Month = month();
  Day = day();
  Hour = hour();
  Minute = minute();
  Second = second();
  DOW = weekday()-1;

  localTimeValid = (Year>2000);
}
