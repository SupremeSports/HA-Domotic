//Variables
//Inputs
extern bool flow;
extern bool high_pres;
extern bool low_pres;

extern int x;
extern int y;

//Analogs
extern int button;
extern int temp_lp;
extern int temp_in;
extern int temp_out;
extern float temp_in_F;
extern float temp_out_F;
extern long temp_cpu;

//Outputs
extern bool fan_out;
extern bool pump_out;

//Others
extern float temp_req;

extern bool disp_on;
extern bool disp_locked;
extern bool spa_mode;

extern bool hold_done;
extern bool hold_button;
extern bool hold_release;
extern int count_hold;

extern int count_display;

extern bool lock_request;
extern bool spa_request;

extern bool button_lock;
extern bool button_spa;
extern bool button_up;
extern bool button_down;

extern bool save_eeprom;


#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

extern SSD1306AsciiWire disp;
