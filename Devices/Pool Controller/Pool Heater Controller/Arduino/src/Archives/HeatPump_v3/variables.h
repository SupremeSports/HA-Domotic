//Variables
//Inputs
extern bool  flow;
extern bool  high_pres;
extern bool  low_pres;

//Analogs
extern int   button;
extern int   temp_lp;
extern int   temp_in;
extern int   temp_out;
extern float temp_in_F;
extern float temp_out_F;
extern long  temp_cpu;

//Outputs
extern bool  fan_out;
extern bool  pump_out;

//Others
extern float temp_req;

extern bool  disp_on;
extern bool  disp_locked;
extern bool  spa_mode;

extern int   seq_count;

extern bool  button_lock;
extern bool  button_spa;
extern bool  button_up;
extern bool  button_down;

extern bool  updn_request;
extern bool  updn_pause;
extern bool  updn_released;

extern bool  save_eeprom;
extern bool  save_done;

extern bool  disp_error;
extern bool  hold_request;
extern bool  hold_release;
extern bool  hold_done;
extern int   hold_count;

extern int   updn_count;
extern int   save_count;

extern bool  heating_disp;

extern bool  done_error;

extern bool  heat_enable;

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

extern SSD1306AsciiWire disp;
