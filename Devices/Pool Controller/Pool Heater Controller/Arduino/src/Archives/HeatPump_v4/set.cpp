#include "arduino.h"
#include "defines.h"
#include "variables.h"

#include "SET.h"

setclass::setclass(){}

void setclass::serial()
{
  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
    Serial.println("[START]");
  #endif
}

void setclass::ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.begin(sizeof Ethernet::buffer, mymac, Ether_cspin);
    ether.staticSetup(myip, gwip, subnet);
  
    ether.printIp("Arduino IP: ", ether.myip);
  
    ether.udpServerListenOnPort(&udpReceive, ListenPort);
  #endif
}

void setclass::oled()
{
  #ifdef OLED_ENABLE
    Wire.begin();         
    disp.begin(&Adafruit128x64, I2C_ADDRESS);
    disp.set400kHz();
  #endif

  //io.SHOW(7);

  //io.SHOW(11);
}

void setclass::setpins()
{
  //Inputs
  pinMode(BUTTONS, INPUT);
  pinMode(TEMP_LP, INPUT);
  pinMode(TEMP_IN_WATER, INPUT);
  pinMode(TEMP_OUT_WATER, INPUT);
  pinMode(SWITCH_FLOW, INPUT);
  pinMode(SWITCH_HP, INPUT);
  pinMode(SWITCH_LP, INPUT);

  //Outputs
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_HEAT_HIGH, OUTPUT);
  pinMode(RELAY_HEAT_ON, OUTPUT);
}

void setclass::eeprom_read()
{
  #ifdef EEPROM_ENABLED
    //READ DATA FROM EEPROM
    //Temperature data is lowered since it cannot accept values larger than 255
    //Temperature has to be multplied by 10 to accept decimal
    //Process same modification while saving data 
    temp_req = TEMP_MIN + ((int)EEPROM.read(0)/10);
      
    int bit_read = (int)EEPROM.read(1);
  
    disp_locked = bitRead(bit_read, 0);
    spa_mode = bitRead(bit_read, 1);
    heat_enable = bitRead(bit_read, 2);
    //spare1 = bitRead(bit_read, 3);
    //spare2 = bitRead(bit_read, 4);
    //spare3 = bitRead(bit_read, 5);
    //spare4 = bitRead(bit_read, 6);
    //spare5 = bitRead(bit_read, 7);
  #else
    //DEFAULT VALUES
    temp_req = TEMP_MIN + 10;
    
    spa_mode = false;
    disp_locked = true;
    heat_enable = true;
    //spare1 = false;
    //spare2 = false;
  #endif

  temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);                     //Make sure temperature is between range
}

void setclass::eeprom_write()
{
  
}

setclass set = setclass();
