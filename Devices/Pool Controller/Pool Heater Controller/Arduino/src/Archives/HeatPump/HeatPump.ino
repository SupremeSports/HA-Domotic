#include <Arduino.h>   
#include <EtherCard.h>                                                                 // Ethernet controller library
#include <IPAddress.h>                                                                 // IPAddress controller library
#include <avr/wdt.h>                                                                   // Watchdog controller library
#include <EEPROM.h>

#include "OLED.h"

// Includes error codes and pins definitions (pin numbers definitiion)
#include "defines.h"

#include "variables.h"

#include "SET.h"

#define ETHERNET_ENABLE
#define OLED_ENABLE

#ifdef ETHERNET_ENABLE
  // Modify the file ip_address_template.h for your own setup and rename to ip_address.h
  // This ensures you personnal data such as IP addresses doesn't get on github to become public
  #include "ip_address.h"
  #include "UDP_receive.h"
#endif

#ifdef OLED_ENABLE
  SSD1306AsciiWire disp;
#endif

#include "misc.h"

//Variables
//Inputs
bool flow = 0;
bool high_pres = 0;
bool low_pres = 0;

//Analogs
int x = 0;
int y = 0;

int button = 0;
int temp_lp = 0;
int temp_in = 0;
int temp_out = 0;
float temp_in_F = 0;
float temp_out_F = 0;
long temp_cpu = 0;

//Outputs
bool fan_out = 0;
bool pump_out = 0;

//Others
float temp_req = 0;

int count;
int count_hold;

long prevMillis_dispoff = millis()+60000;

bool disp_on = true;

bool eeprom_req_saved = true;
bool disp_saved = true;
int spalock_modes = 0;

bool button_lock = false;
bool disp_locked = false;
bool lock_request = false;
bool lock_hold = false;

bool button_spa = false;
bool spa_mode = false;
bool spa_request = false;
bool spa_hold = false;

long prevMillis_fan = 0;
bool fan_off_wait = false;

bool fivelines_on = false;
bool threelines_on = false;

bool alarm_on = false;

bool heat_enable = false;

bool button_up = false;
bool button_down = false;
bool updown_request = false;

void setup()
{
  wdt_disable();
  wdt_enable(WDTO_8S);

  Serial.begin(9600);
  Serial.println("[START]");

  #ifdef ETHERNET_ENABLE
    bool test = ether.begin(sizeof Ethernet::buffer, mymac, Ether_cspin);
    ether.staticSetup(myip, gwip, subnet);
  
    ether.printIp("Arduino IP: ", ether.myip);
  
    ether.udpServerListenOnPort(&udpReceive, ListenPort);
  #endif

  #ifdef OLED_ENABLE
    Wire.begin();         
    disp.begin(&Adafruit128x64, I2C_ADDRESS);
    disp.set400kHz();  
  #endif
  
  //setpins();
  SET.SETPINS();
}

void loop()
{
  #ifdef ETHERNET_ENABLE
    ether.packetLoop(ether.packetReceive());
  #endif

  wdt_reset();
  
  readsensors();

  if((button > 20) || updown_request || lock_hold || spa_hold)
    mainprog();
  else
  {
    pumpoutput();
    fanoutput();

    if(alarm_on)
      alarmprog();
    else
      mainprog();
  }
}

//MAIN PROGRAM
// This function detects button pressed and controls the display accordingly
void mainprog()
{
  if((button > 20) || updown_request || lock_hold || spa_hold)
  {
    buttonpressed();
    return;
  }
  else if(button < 20 && !eeprom_req_saved)
    savetoEEPROM();

  if((button < 20) && eeprom_req_saved)
    heatstatus(disp_saved);

  
}

//Alarms to be placed in priority order
// Alarms can display four (4) characters MAX
void alarmprog()
{
  #ifdef OLED_ENABLE
    if(temp_lp < temp_lp_low)
    {
      OLED.CHAR(0, 1, "ERR: LOW PRES FREEZE", 1, true);
      OLED.CHAR(0, 2, "T LP ", 4, true);
    }
    else if(low_pres)
    {
      OLED.CHAR(0, 1, "ERR: LOW PRES ISSUE", 1, true);
      OLED.CHAR(0, 2, "LP   ", 4, true);
    }
    else if(!high_pres)
    {
      OLED.CHAR(0, 1, "ERR: HIGH PRES ISSUE", 1, true);
      OLED.CHAR(0, 2, "HP   ", 4, true);
    }
    else if(flow)
    {
      OLED.CHAR(0, 1, "ERR: WATER FLOW ISSUE", 1, true);
      OLED.CHAR(0, 2, "FLO ", 4, true);
    }
    else
    {
      OLED.CHAR(0, 1, "ERR: NOT DEFINED", 1, true);
      OLED.CHAR(0, 2, "---- ", 4, true);
    }
  #endif
}



//PUMP OUTPUT CONTROL
// This function controls the pump output
// Pump runs whenever 
void pumpoutput()
{
  float temp_req_hi;
  float temp_req_lo;
  //Adjust min/max accordingly to spa/pool mode
  if(spa_mode)
  {
    temp_req_lo = temp_req - 0.2;
    temp_req_hi = temp_req + 1.0;
  }
  else
  {
    temp_req_lo = temp_req - 0.5;
    temp_req_hi = temp_req + 0.5;
  } 

  if(((temp_lp > temp_lp_low) && low_pres && high_pres && flow && heat_enable) || true)
  {
    if(temp_in_F <= temp_req_lo)
      pump_out = true;
    if(temp_in_F >= temp_req_hi)
      pump_out = false;
    
    alarm_on = false;
  }
  else
  {
    pump_out = false;
    
    //alarm_on = true;
  }
  
  digitalWrite(RELAY_HEAT, pump_out);
}

//FAN OUTPUT CONTROL
// This function controls the fan output
// Fan runs as soon a the pump runs
// Fan continues to run for 30s after pump stops
void fanoutput()
{
  if(pump_out || (temp_lp < temp_lp_low))
    fan_out = true;
  else
  {
    if(fan_out)
    {
      if(fan_off_wait)
      {
        if((millis() - prevMillis_fan) > FAN_OFF_DELAY)
        {
          fan_out = false;
          fan_off_wait = false;
        }
      }
      else
      {
        prevMillis_fan = millis();
        fan_off_wait = true;
      }
    }
  }

  digitalWrite(RELAY_FAN, fan_out);
}

void buttonpressed()
{
  button_lock = (button >= Lock_min && button <= Lock_max);
  button_up = (button >= Up_min && button <= Up_max);
  button_down = (button >= Down_min && button <= Down_max);
  button_spa = (button >= Spa_min && button <= Spa_max);

  #ifdef OLED_ENABLE
    if(!disp_on)
      OLED.DISPON();
  #endif

  if(disp_locked && (button_up || button_down || button_spa))
  {
    OLED.CHAR(0, 1, "Buttons are locked", 1, true);
    OLED.CHAR(0, 2, "LOCK ", 4, true);
    delay(DISP_CHG_DELAY);
    return;
  }

  if(button < 20)
    count = 0;
    
  wdt_reset();

  //LOCK BUTTON CONTROL
  if(button_lock)
  {
    if(!lock_hold && count_hold == 0)
    {
      OLED.CHAR(0, 1, "Please hold button...", 1, true);
      OLED.CHAR(0, 2, "HOLD ", 4, true);
      lock_hold = true;
      delay(DISP_HLD_DELAY);
    }
    else if(!lock_request)
    {
      if(!disp_locked)
      {
        disp_locked = true; 
        #ifdef OLED_ENABLE
          OLED.CHAR(0, 1, "Buttons locked", 1, true);
          OLED.CHAR(0, 2, "LOCK  ", 4, true);
        #endif
      }
      else
      {
        disp_locked = false; 
        #ifdef OLED_ENABLE
          OLED.CHAR(0, 1, "Buttons unlocked", 1, true);
          OLED.CHAR(0, 2, "UNLK  ", 4, true);
        #endif
      }
      lock_request = true;
      eeprom_req_saved = false;
    }
  }
  else
  {
    lock_request = false;
    lock_hold = false;
    delay(500);
  }

  //SPA MODE BUTTON CONTROL
  if(button_spa)
  {
    if(!spa_hold)
    {
      OLED.CHAR(0, 1, "Please hold button...", 1, true);
      OLED.CHAR(0, 2, "HOLD ", 4, true);
      spa_hold = true;
      delay(DISP_HLD_DELAY);
    }
    else if(!spa_request)
    {
      if(!spa_mode)
      {
        spa_mode = true; 
        #ifdef OLED_ENABLE
          OLED.CHAR(0, 1, "SPA mode enabled", 1, true);
          OLED.CHAR(0, 2, "SPA  ", 4, true);
        #endif
      }
      else
      {
        spa_mode = false; 
        #ifdef OLED_ENABLE
          OLED.CHAR(0, 1, "POOL mode enabled", 1, true);
          OLED.CHAR(0, 2, "POOL ", 4, true);
        #endif
      }
      spa_request = true;
      eeprom_req_saved = false;
    }
  }
  else
  {
    spa_request = false;
    spa_hold = false;
    delay(500);
  }
}

#ifdef ETHERNET_ENABLE
void udpReceive(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len)
{
  IPAddress src(src_ip[0],src_ip[1],src_ip[2],src_ip[3]);

  SourcePort = src_port;

  parseData(String(data));

  callSubfunction();
  
  UDPreply();
}
#endif

