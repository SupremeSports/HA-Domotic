//Variables
//Inputs
extern bool  flow;
extern bool  high_pres;
extern bool  low_pres;
  
extern int   temp_lp;
extern float temp_in_F;
extern float temp_out_F;

//Outputs
extern bool  fan_out;
extern bool  fan_high;
extern bool  pump_out;

//Others
extern float temp_req;

//Base settings
extern bool  disp_on;
extern bool  disp_locked;
extern bool  spa_mode;
extern bool  heat_enable;

extern int   seq_count;

extern bool  alarm_on;
extern int   alarm_bits;

extern bool  save_eeprom;

extern bool  heating_on;
extern bool  heating_off;
extern bool  heating_stop;

#include <avr/wdt.h>                                                                   // Watchdog controller library
/*
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

extern SSD1306AsciiWire disp;*/
