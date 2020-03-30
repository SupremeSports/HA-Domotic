#include <Arduino.h>   
#include <EtherCard.h>                                                                 // Ethernet controller library
#include <IPAddress.h>                                                                 // IPAddress controller library
#include <avr/wdt.h>                                                                   // Watchdog controller library
#include <EEPROM.h>

#include "defines.h"

#include "variables.h"

//Variables
//Inputs
bool flow = 0;
bool high_pres = 0;
bool low_pres = 0;

int button = 0;
int temp_lp = 0;
int temp_in = 0;
int temp_out = 0;
float temp_in_F = 0;
float temp_out_F = 0;

//Outputs
bool fan_out = 0;
bool pump_out = 0;

//Others
float temp_req = 0;

bool disp_on = true;
bool disp_locked;
bool spa_mode;

bool hold_done;
bool hold_button;
bool hold_release;
int count_hold;

int count_display = 5;

bool lock_request;
bool spa_request;

bool button_lock;
bool button_spa;
bool button_up;
bool button_down;

bool save_eeprom;

#include "SET.h"
#include "IO.h"

//#define ETHERNET_ENABLE
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

void setup()
{
  //wdt_enable(WDTO_8S);

  Serial.begin(9600);
  Serial.println("[START]");

  #ifdef ETHERNET_ENABLE
    ether.begin(sizeof Ethernet::buffer, mymac, Ether_cspin);
    ether.staticSetup(myip, gwip, subnet);
  
    ether.printIp("Arduino IP: ", ether.myip);
  
    ether.udpServerListenOnPort(&udpReceive, ListenPort);
  #endif

  #ifdef OLED_ENABLE
    Wire.begin();         
    disp.begin(&Adafruit128x64, I2C_ADDRESS);
    disp.set400kHz();
  #endif

  set.SETPINS();

}

void loop()
{
  #ifdef ETHERNET_ENABLE
    ether.packetLoop(ether.packetReceive());
  #endif

  wdt_reset();
  
  //update_display();

  Serial.println(count_display);

  delay(500);
}

void update_display()
{
  do
  {
    io.READBUTTONS();

    if(hold_done || hold_release)
      break;

    if((button_spa || button_up || button_down) && disp_locked)
      io.BUTTONFAIL();
    else
    {
      if(button_lock || button_spa)
      {
        if(!hold_button || (hold_button && !hold_done))
        {
          io.BUTTONHOLD();
          io.SHOW(HOLD);
        }
      }
      else if(!button_lock && !button_spa && hold_button)
      {
        hold_button = false;
        if(count_display < 5)
          count_display = 5;
      }
    }
  }while(button_lock || button_spa || hold_button);

  if((lock_request || spa_request) && hold_button)
    io.BUTTONDONE();

  io.READSENSORS();

  io.SHOW(5);

  if(count_display == 5)
  {
    io.SHOW(2);
    io.SHOW(3);
    io.SHOW(4);
  }
  if(count_display == 6)
    io.SHOW(3);

  if(count_display == 0 && save_eeprom)
    io.SHOW(6);

  count_display++;
  if(count_display == 10 || count_display >= 15)
    count_display = 6;
}

