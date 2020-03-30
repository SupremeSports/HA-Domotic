#include "defines.h"
#include "variables.h"

#include "UDP_Receive.h"

#include "show.h"
#include "user.h"
#include "io.h"

#include "set.h"

float  Temp_Inlet;
float  Temp_Solar;
float  Temp_Middle;
float  Temp_Heater;
float  Temp_Return;
float  Temp_Roof;

float  Sun_Level;
float  Water_Pres;

int    Pot_Mtr[3];
int    Percent_Mtr[3];
bool   Sw_Mtr[3];

bool   Raining;

String datetime;
bool   disp_on;
bool   disp_locked;

bool   manual[3];
bool   automatic[3];
bool   disabled[3];

int    seq_count;

bool   save_eeprom;

bool   heat_enable;

uint8_t servonum = 0;

//Setting to negative values to make sure the servos are refreshed on boot
int    Prev_Angle_Mtr[3] = {-1, -1, -1};

float  temp_req;               //Requested temperature

bool   solar_enable = true;
bool   heatpump_enable = true;

void setup()
{
  set_ethernet();             //Initialize ethernet if defined (see defines.h - ETHERNET_ENABLE)
  
  wdt_enable(WDTO_8S);        //Enable watchdog timer at 8s. If board stops responding for more than 8 seconds, it will reset by itslef
  
  set.serial();               //Initialize serial port if defined (see defines.h - DEBUG_ENABLE)

  set.setpins();              //Initialize I/O pins
  
  io.set_mux();               //Set Analog MUX settings (to be sent into set.setpins()
  io.set_pwm();               //Set PWM MUX settings (to be sent into set.setpins()

  set.eeprom_read();          //Read EEPROM values to initialize process if defined (see defines.h - EEPROM_ENABLED)

  show.set();                 //Initialize oled display IIC port if defined (see defines.h - OLED_ENABLE)

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

  io.move_motors();           //Control valves positions
}
