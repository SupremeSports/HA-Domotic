//Variables
//Digital Inputs
extern bool  flow;
extern bool  high_pres;
extern bool  low_pres;

//Analog Inputs
extern int   temp_lp_raw;
extern float temp_in_F;
extern float temp_out_F;

//Digital Outputs
extern bool  fan_out;
extern bool  fan_high;
extern bool  pump_out;

//Others
extern float temp_req;

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

extern String datetime;

extern int   prev_alarm;

extern long  prevMillis_heat;

#include <avr/wdt.h>                                                                   // Watchdog controller library

