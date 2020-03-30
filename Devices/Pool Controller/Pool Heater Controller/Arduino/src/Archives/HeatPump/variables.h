
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

extern int count;
extern int count_hold;

extern long prevMillis_dispoff;
extern bool disp_off_wait;

extern bool disp_on;

extern bool eeprom_req_saved;
extern bool disp_saved;
extern int spalock_modes;

extern bool button_lock;
extern bool disp_locked;
extern bool lock_request;
extern bool lock_hold;

extern bool button_spa;
extern bool spa_mode;
extern bool spa_request;
extern bool spa_hold;

extern long prevMillis_fan;
extern bool fan_off_wait;

extern bool fivelines_on;
extern bool threelines_on;

extern bool alarm_on;

extern bool heat_enable;

extern bool button_up;
extern bool button_down;
extern bool updown_request;

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

extern SSD1306AsciiWire disp;
