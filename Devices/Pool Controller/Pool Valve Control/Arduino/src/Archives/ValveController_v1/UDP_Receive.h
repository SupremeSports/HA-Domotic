// Modify the file ip_address_template.h for your own setup and rename to ip_address.h
// This ensures you personnal data such as IP addresses doesn't get on github to become public
#include <EtherCard.h>                                                                 // Ethernet controller library
#include <IPAddress.h>                                                                 // IPAddress controller library
#include "ip_address.h"

#include "show.h"

void callSubfunction()
{
  String command = commands[0];
  command.toLowerCase();

  if(command == "datetime")             //Receive date/time from server
  {
    datetime = commands[1];
    
    if(disp_on)
      show.date();

    sprintf(str, "%d", 1);
  }
  else if(command == "heaton")          //Enable/Disable heating
  {
    heat_enable = (commands[1] == "1") ? true : false;

    if(temp_req < TEMP_MIN+1)
      temp_req == TEMP_DEF;
    
    save_eeprom = true;
    seq_count = DISP_BASE_NOW;
    
    sprintf(str, "%d", 1);
  }
  else if(command == "heatset")         //Receive temperature command from server
  {
    temp_req = commands[1].toFloat(); //*
    temp_req = constrain(temp_req, TEMP_MIN+1, TEMP_MAX);
    
    save_eeprom = true;
    seq_count = DISP_BASE_NOW;
    sprintf(str, "%d", 1);
  }
  else if(command == "status")          //Send heat pump system status to server
  {
    sprintf(str, "%d;%d;%d;%d;%d;%d;%s;%s;%s", Percent_Mtr[0], Percent_Mtr[1], Percent_Mtr[2], manual[0], manual[1], manual[2], String(Temp_Inlet,1).c_str(), String(Temp_Middle,1).c_str(), String(Temp_Return,1).c_str());
  }
  else
  {
    sprintf(str, "%s", "ERROR");
  }
}

void parseData(String stringdata)
{
  int delimiters[MAX_COMMANDS];
  int delimiter_qty;
  
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

void UDPreply(int SourcePort)
{
  #ifdef ETHERNET_ENABLE
    char replyUDP[BUFLEN];
    strcpy(replyUDP, str);
  
    ether.makeUdpReply(replyUDP, strlen(replyUDP), SourcePort);
  #endif
}

void loop_ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.packetLoop(ether.packetReceive());
  #endif
}

//callback that prints received packets to the serial port
void udpReceive(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len)
{
  #ifdef ETHERNET_ENABLE
    IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);
  
    parseData(String(data));
  
    callSubfunction();
    
    UDPreply(src_port);
  #endif
}

void set_ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.begin(sizeof Ethernet::buffer, mymac, Ether_cspin);
    ether.staticSetup(myip, gwip, subnet);
    ether.udpServerListenOnPort(&udpReceive, ListenPort);
    #ifdef DEBUG_ENABLE
      ether.printIp("Arduino IP: ", ether.myip);
    #endif
  #endif
}
