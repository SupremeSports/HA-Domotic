#include <Arduino.h>   
#include <EtherCard.h>                                                                 // Ethernet controller library
#include <IPAddress.h>                                                                 // IPAddress controller library
#include <avr/wdt.h>                                                                   // Watchdog controller library

#include "defines.h"

#include "variables.h"

//Variables
//Inputs
bool  flow = false;
bool  high_pres = false;
bool  low_pres = false;

int   button = 0;
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

bool  disp_on = true;
bool  disp_locked = true;
bool  spa_mode;

int   seq_count = 0; // Initialize display sequence counter

bool  button_lock;   // Lock button status
bool  button_spa;    // Spa button status
bool  button_up;     // Up button status
bool  button_down;   // Down button status

bool  updn_request;
bool  updn_pause;
bool  updn_released;

bool  save_eeprom;
bool  save_done;

bool  disp_error;
bool  hold_request;
bool  hold_release;
bool  hold_done;
int   hold_count;

int   updn_count;
int   save_count;

bool  heating_disp = false;

bool  done_error;

bool  heat_enable;

#include "SET.h"
#include "IO.h"

#ifdef ETHERNET_ENABLE
  // Modify the file ip_address_template.h for your own setup and rename to ip_address.h
  // This ensures you personnal data such as IP addresses doesn't get on github to become public
  #include "ip_address.h"
  #include "UDP_receive.h"
#endif

#ifdef OLED_ENABLE
  SSD1306AsciiWire disp;    //Declar variable disp
#endif

void setup()
{
  //wdt_enable(WDTO_8S);     //Enable watchdog timer at 8s. If board stops responding for more than 8 seconds, it will reset by itslef

  init_serial();           //Initialize serial port if defined (see defines.h - OLED_ENABLE)

  init_ethernet();         //Initialize ethernet port if defined (see defines.h - ETHERNET_ENABLE)

  init_oled();             //Initialize oled display IIC port

  io.SETPINS();           //Set pins mode for functionnalities (inputs, outputs, etc.)
  io.READ_EEPROM();        //Read preset values from EEPROM
}

void loop()
{
  loop_ethernet();         //Verify if ethernet port has received data

  wdt_reset();             //Reset watchdog timer to prevent unwanted reset
  
  hold_button();           //Verify and manage buttons being held

  io.READSENSORS();        //Read all sensor values

  sequence();              //Run sequenced display
}

void hold_button()
{
  do
  {
    io.READBUTTONS(); // Read buttons staus

    io.BUTTONUPDOWN();

    if(hold_release || done_error || save_eeprom)
      break;

    io.BUTTONHOLD();  // Loops until button has been held long enough (~2sec)
    io.BUTTONDONE();  // Verify if button hold (lock or spa) has completed
    io.BUTTONLOCK();  // Verify if pressed button is authorized (keypad NOT locked)

    wdt_reset();      //Reset watchdog timer to prevent unwanted reset
    
  }while(button > 20 || updn_request);
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
    io.SHOW(2);
    if(!save_eeprom)
      io.SHOW(4);   //Display SPA/POOL mode and LOCKED symbol
    
    seq_count = DISP_RUN;   //Jump to normal sequence right away
  }

  if(!save_eeprom && !save_done && (seq_count >= DISP_BASE_NOW))
    io.SHOW(DISP_HEAT); //Update temperature reading once per cycle (~1sec)
  
  if(seq_count == DISP_RUN)
  {
    Serial.println("DISPLAY TEMPERATURE");
    io.SHOW(3);
  }

  Serial.println(seq_count);

  seq_count++;

  if(seq_count >= DISP_MAX)
  {
    seq_count = DISP_RUN;
    
    save_data();
  }
}

void save_data()
{
  if(save_eeprom)
  {
    io.SAVE_EEPROM();
    io.SHOW(14);
  }
  else
  {
    if(save_done)
      io.SHOW(4);   //Display SPA/POOL mode and LOCKED symbol
    save_done = false;
  }

  save_eeprom = false;
}

void init_serial()
{
  #ifdef DEBUG_ENABLE
    Serial.begin(9600);
    Serial.println("[START]");
  #endif
}

void init_oled()
{
  #ifdef OLED_ENABLE
    Wire.begin();         
    disp.begin(&Adafruit128x64, I2C_ADDRESS);
    disp.set400kHz();
  #endif

  io.SHOW(7);

  io.SHOW(11);
}

void init_ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.begin(sizeof Ethernet::buffer, mymac, Ether_cspin);
    ether.staticSetup(myip, gwip, subnet);
  
    ether.printIp("Arduino IP: ", ether.myip);
  
    ether.udpServerListenOnPort(&udpReceive, ListenPort);
  #endif
}

void loop_ethernet()
{
  #ifdef ETHERNET_ENABLE
    ether.packetLoop(ether.packetReceive());
  #endif
}
