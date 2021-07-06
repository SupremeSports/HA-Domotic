// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- TIME FUNCTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//IMPORTANT - Always send a delimiter at the of string for parsing to succeed
//Expected payload: "YYYY:MM:DD:hh:mm:ss:"
void parseTime(char* message)
{
  int dataSize = 6;
  uint16_t dateTime[dataSize]; // ALWAYS END WITH A DELIMITER
  char data[5];

  dateTime[0] = atoi(strncpy(data, message, 4));
  int k = 1;
  for (int i=4; i<18; i++)    //dateTime array
  {
    for (int j=0; j<2; j++)   //character array
    {
      i++; //Skip delimiter and/or go to next character
      data[j] = message[i];
    }
    data[2] = '\0';
    dateTime[k] = atoi(data);
    k++; //Next parsing
  }

  setTime(dateTime[3], dateTime[4], dateTime[5], dateTime[2], dateTime[1], dateTime[0]);

  updateTime();

  localTimeValid = (Year>2000);
}

void updateTime()
{
  ONS_1s = false;
  ONS_5s = false;
  ONS_10s = false;
  ONS_1m = false;
  if (second() == Second)
    return;

  if (minute() != Minute)
    ONS_1m = true;
  
  Year = year();
  Month = month();
  Day = day();
  Hour = hour();
  Minute = minute();
  Second = second();
  DOW = weekday()-1;

  localTimeValid = (Year>2000);

  ONS_1s = true;
  ONS_5s = Second%5==0;
  ONS_10s = Second%10==0;

  PRE_5s = (Second+1)%5==0 || ONS_5s; //Less than a second before next trigger
}
