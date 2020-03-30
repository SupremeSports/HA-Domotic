#include <Arduino.h>   
//#include <avr/wdt.h>                                                                   // Watchdog controller library

#include "defines.h"
#include "variables.h"
#include "UDP_Receive.h"

//#include "ip_address.h"

#include "show.h"
#include "user.h"
#include "io.h"

#include "set.h"

//Variables
//Digital Inputs
bool  flow;
bool  high_pres;
bool  low_pres;

//Analog Inputs
int   temp_lp;
float temp_in_F;
float temp_out_F;

//Digital Outputs
bool  fan_out = false;
bool  fan_high = false;
bool  pump_out = false;

//Others
float temp_req = TEMP_DEF;

//Base settings
bool  disp_on;
bool  disp_locked;
bool  spa_mode;
bool  heat_enable;

//Display sequence counter
int   seq_count;

//Alarms checking
bool  alarm_on;
int   alarm_bits;

bool  save_eeprom;

bool  heating_on;
bool  heating_off;
bool  heating_stop;

void setup()
{
  set_ethernet();
  
  wdt_enable(WDTO_8S);        //Enable watchdog timer at 8s. If board stops responding for more than 8 seconds, it will reset by itslef

  set.serial();               //Initialize serial port if defined (see defines.h - OLED_ENABLE)
  
  set.setpins();              //Initialize I/O pins

  set.eeprom_read();          //Read EEPROM values to initialize process

  disp_on = show.set();       //Initialize oled display IIC port

  show.initial();             //Initialize display and show splashscreen

  seq_count = DISP_BASE_NOW;  //Jump to normal running display
}

void loop()
{
  wdt_reset();                //Reset watchdog timer to prevent unwanted reset

  loop_ethernet();            //ether.packetLoop(ether.packetReceive());

  user.read_buttons();        //Read buttons
  user.button_work();         //Process actions according to button selection

  io.read_sensors();          //Read sensors and determine alarms

  if(!alarm_on)
    io.sequence();            //Run normal display sequence and refresh

  io.pump();                  //Control heat pump relay
  io.fan();                   //Control fan relay
}


