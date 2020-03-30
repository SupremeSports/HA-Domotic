#include <Arduino.h>

#include "defines.h"
#include "variables.h"

#include "UDP_Receive.h"

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
int   temp_lp_raw;
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

//EEPROM save request
bool  save_eeprom;

//Heating display mode
bool  heating_on;
bool  heating_off;
bool  heating_stop;

String datetime;

int   prev_alarm;

long  prevMillis_heat;

void setup()
{
  set_ethernet();
  
  wdt_enable(WDTO_8S);        //Enable watchdog timer at 8s. If board stops responding for more than 8 seconds, it will reset by itslef

  set.serial();               //Initialize serial port if defined (see defines.h - OLED_ENABLE)
  
  set.setpins();              //Initialize I/O pins

  set.eeprom_read();          //Read EEPROM values to initialize process

  show.set();                 //Initialize oled display IIC port

  show.initial();             //Initialize display and show splashscreen
}

void loop()
{
  wdt_reset();                //Reset watchdog timer to prevent unwanted reset

  loop_ethernet();            //Verify if new packet has been received

  user.read_buttons();        //Read buttons
  user.button_work();         //Process actions according to button selection

  io.read_sensors();          //Read sensors and determine alarms

  io.sequence();              //Run normal display sequence and refresh

  io.pump();                  //Control heat pump relay
  io.fan();                   //Control fan relay
}


