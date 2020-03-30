//Variables
//Inputs
extern bool  flow;
extern bool  high_pres;
extern bool  low_pres;

extern int   button;
  extern bool  button_lock;
  extern bool  button_spa;
  extern bool  button_up;
  extern bool  button_down;
  
extern int   temp_lp;
extern int   temp_in;
extern int   temp_out;
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



extern bool  error_done;
extern bool  hold_done;
extern int   button_count;

extern int   seq_count;

extern bool  alarm_on;

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

extern SSD1306AsciiWire disp;
