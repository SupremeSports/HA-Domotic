void callSubfunction()
{
  String command = commands[0];
  command.toLowerCase();
  int level = 0;
  int pin = 0;

  if(command == "datetime")
  {
    OLED.CHAR(0, 0, commands[1], 1, false);

    sprintf(str, "%d", 1);
  }
  else if(command == "heaton")
  {
    heat_enable = (commands[1] == "1") ? true : false;
    
    save_eeprom = true;
    
    sprintf(str, "%d", 1);
  }
  else if(command == "heatset")
  {
    temp_req = commands[1].toFloat();
    temp_req = constrain(temp_req, temp_min, temp_max);
    
    save_eeprom = true;

    sprintf(str, "%d", 1);
  }
  else if(command == "cursor")
  {
    x = commands[1].toInt();
    y = commands[2].toInt();
  }
  else if(command == "heatmode")
  {
    spa_mode = (commands[1] == "1") ? true : false;
    
    save_eeprom = true;

    sprintf(str, "%d", 1);
  }
  else if(command == "status")
  {
    sprintf(str, "%s;%s;%s;%s", (pump_out ? "RUN" : "STOP"),String(temp_req,1).c_str(),String(temp_in_F,1).c_str(),String(temp_out_F,1).c_str(), (alarm_on ? "ALARM" : "SYS OK"));
  }
  else
  {
    sprintf(str, "%s", "ERROR");
  }  
}

void parseData(String stringdata)
{  
  // Blank all string tables
  for(int i=0; i<MAX_COMMANDS; i++)
  {
    delimiters[i] = 0;
    commands[i] = "";
  }

  // Find all delimiters positions in received string
  delimiter_qty = 0;
  for(int i=0; i<MAX_COMMANDS; i++)
  {
    if(i==0)
      delimiters[i] = stringdata.indexOf(';');
    else
      delimiters[i] = stringdata.indexOf(';', delimiters[i-1]+1);
    
    if(delimiters[i]<=0)
      break;

    delimiter_qty += 1;
  }

  // Split and store data according to delimiters positions
  for(int i=0; i<=delimiter_qty; i++)
  {
    if(i==0)
      commands[i] = stringdata.substring(0, delimiters[i]);
    else if(i==delimiter_qty)
      commands[i] = stringdata.substring(delimiters[i-1]+1);
    else
      commands[i] = stringdata.substring(delimiters[i-1]+1, delimiters[i]);
  }
}

void UDPreply()
{
  strcpy(replyUDP, str);

  ether.makeUdpReply(replyUDP, strlen(replyUDP), SourcePort);
}
