#include <Arduino.h>   
#include <avr/wdt.h>                                                                   // Watchdog controller library

#include "SHOW.h"

#include "defines.h"
#include "variables.h"

//Variables
//Inputs
bool  flow = false;
bool  high_pres = false;
bool  low_pres = false;

int   button = 0;
  bool  button_lock;   // Lock button status
  bool  button_spa;    // Spa button status
  bool  button_up;     // Up button status
  bool  button_down;   // Down button status
  
int   temp_lp = 0;
int   temp_in = 0;
int   temp_out = 0;
float temp_in_F = 0;
float temp_out_F = 0;

//Outputs
bool  fan_out = false;
bool  fan_high = false;
bool  pump_out = false;

//Others
float temp_req = 0;

//Base settings
bool  disp_on = true;
bool  disp_locked = true;
bool  spa_mode = false;
bool  heat_enable = true;

bool  error_done = false;
bool  hold_done = false;
int   button_count;

int   seq_count;

bool  alarm_on = false;

#include "SET.h"

#ifdef ETHERNET_ENABLE
  // Modify the file ip_address_template.h for your own setup and rename to ip_address.h
  // This ensures you personnal data such as IP addresses doesn't get on github to become public
  #include <EtherCard.h>                                                                 // Ethernet controller library
  #include <IPAddress.h>                                                                 // IPAddress controller library
  #include "ip_address.h"
  #include "UDP_receive.h"
#endif

#ifdef OLED_ENABLE
  SSD1306AsciiWire disp;    //Declar variable disp
#endif

void setup()
{
  //wdt_enable(WDTO_8S);      //Enable watchdog timer at 8s. If board stops responding for more than 8 seconds, it will reset by itslef

  set.serial();               //Initialize serial port if defined (see defines.h - OLED_ENABLE)

  set.ethernet();             //Initialize ethernet port if defined (see defines.h - ETHERNET_ENABLE)

  set.oled();                 //Initialize oled display IIC port

  set.setpins();              //Initialize I/O pins

  set.eeprom_read();          //Read EEPROM values to initialize process

  show.initial();             //Initialize display and show splashscreen

  seq_count = DISP_BASE_NOW;  //Jump to normal running display
}

void loop()
{
  loop_ethernet();            //Verify if ethernet port has received data

  wdt_reset();                //Reset watchdog timer to prevent unwanted reset

  read_buttons();             //Read buttons
  button_work();              //Process actions according to button selection

  read_sensors();             //Read sensors and determine alarms

  if(!alarm_on)
    sequence();               //Run normal display sequence
  else
    alarm();                  //Display alarms

  heat_control();             //Control heat pump relay
  fan_control();              //Control fan relay
}

void sequence()
{
  //if(seq_count == DISP_BASE_LONG)
    //Do nothing for a long delay (typically four cycles)
  
  //if(seq_count == DISP_BASE_SHORT)
    //Do nothing for a short delay (typically two cycles)
    
  if(seq_count == DISP_BASE_NOW)
  {
    Serial.println("DISPLAY BASE");
    if(disp_on)
    {
      show.temps_base();
      show.modes(disp_locked, spa_mode);
    }
    
    seq_count = DISP_RUN;   //Jump to normal sequence right away
  }

  if(seq_count == DISP_RUN)
  {
    if(disp_on)
      show.temps(temp_req, temp_in_F, temp_out_F);
  }

  if(seq_count >= DISP_RUN)
  {
    if(disp_on)
      show.heating(seq_count);
  }

  Serial.println(seq_count);

  seq_count++;

  if(seq_count >= DISP_MAX)
    seq_count = DISP_RUN;

  delay(500);
}

void alarm()
{
  
}

void heat_control()
{
  
}

void fan_control()
{
  
}

//Remains in the loop until buttons released or hold timeout occurs
//Reset WDT frequently while inside this loop
void read_buttons()
{
  button = (int)analogRead(BUTTONS);

  //Convert analog buttons to actual boolean buttons
  button_lock = (button >= Lock_min && button <= Lock_max);
  button_up = (button >= Up_min && button <= Up_max);
  button_down = (button >= Down_min && button <= Down_max);
  button_spa = (button >= Spa_min && button <= Spa_max);
}

void button_work()
{
  if(button < 20)
  {
    error_done = false;
    hold_done = false;
    
    return;
  }

  disp_on = true;

  if(error_done || hold_done)
    return;
  
  if((button_spa || button_up || button_down) && disp_locked && !error_done)
    error();

  if((button_lock || (button_spa && !disp_locked)) && !hold_done)
    hold();

  if((button_up || button_down) && !hold_done && !disp_locked)
    updown();
}

void hold()
{
  button_count = 0;
  Serial.println("PLEASE HOLD");

  show.hold();

  do
  {
    Serial.println("BUTTON HOLD");
    delay(100);
    
    read_buttons();

    if(button_count > 10)
      hold_done = true;

    button_count++;
  }
  while((button_lock || button_spa) && !hold_done);

  if(hold_done && button_lock)
    lock();
  if(hold_done && button_spa)
    spa();

  if(!hold_done)
    seq_count = DISP_BASE_NOW;
}

void updown()
{
  button_count = 0;
  Serial.println("UP/DOWN BUTTONS");
  delay(100);
  
  show.temp_req_base();

  do
  {    
    if(button_up)
    {
      temp_req += 0.5;
      button_count = 0;
    }
    if(button_down)
    {
      temp_req -= 0.5;
      button_count = 0;
    }

    temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);

    show.temp_req(temp_req);

    while(button_up || button_down)
    {
      delay(100);
      read_buttons();
      button_count++;

      if(button_count > 10)
        break;

      if(!button_up && !button_down)
        button_count = 0;
    };

    if(button_count > 10)
      hold_done = true;

    button_count++;

    read_buttons();

    wdt_reset();
  }
  while(!hold_done);

  delay(1000);
  seq_count = DISP_BASE_NOW;
}

void lock()
{
  if(disp_locked)
  {
    disp_locked = false; 
    Serial.println("UNLOCKED");
  }
  else
  {
    disp_locked = true; 
    Serial.println("LOCKED");
  }
  show.lock(disp_locked);
  delay(1000);
  seq_count = DISP_BASE_NOW;
}

void spa()
{
  if(spa_mode)
  {
    spa_mode = false; 
    Serial.println("POOL MODE");
  }
  else
  {
    spa_mode = true; 
    Serial.println("SPA MODE");
  }
  show.spa(spa_mode);
  delay(1000);
  seq_count = DISP_BASE_NOW;
}

void error()
{
  button_count = 0;
  Serial.println("DISP LOCKED");

  show.error();
  
  do
  {
    Serial.println("ERROR HOLD");
    delay(100);
  
    read_buttons();

    if(button_count > 10)
      error_done = true;
    
    button_count++;
  }
  while(button > 20 && !error_done);

  Serial.println("ERROR RELEASED");
  seq_count = DISP_BASE_NOW;
}

void read_sensors()
{
  temp_in = (int)analogRead(TEMP_IN_WATER);
  temp_in_F = (ratio_ax2 * temp_in * temp_in) + (ratio_bx * temp_in) + ratio_c;
  
  temp_out = (int)analogRead(TEMP_OUT_WATER);
  temp_out_F = (ratio_ax2 * temp_out * temp_out) + (ratio_bx * temp_out) + ratio_c;

  //Safety devices
  flow = digitalRead(SWITCH_FLOW);
  high_pres = digitalRead(SWITCH_HP);
  low_pres = digitalRead(SWITCH_LP);
  temp_lp = (int)analogRead(TEMP_LP);
}

void loop_ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.packetLoop(ether.packetReceive());
  #endif
}
