//Variables

extern float  Temp_Inlet;
extern float  Temp_Solar;
extern float  Temp_Middle;
extern float  Temp_Heater;
extern float  Temp_Return;
extern float  Temp_Roof;

extern float  Sun_Level;
extern float  Water_Pres;

extern int    Pot_Mtr[3];
extern int    Percent_Mtr[3];
extern bool   Sw_Mtr[3];
extern bool   Enet_Pot_Mtr[3];
extern bool   Enet_Sw_Mtr[3];

extern bool   Raining;

extern String datetime;
extern bool   disp_on;
extern bool   disp_locked;

extern int    Prev_Angle_Mtr[3];

extern float  temp_req;

extern bool   solar_enable;
extern bool   heatpump_enable;

extern bool   manual[3];
extern bool   automatic[3];
extern bool   disabled[3];

extern int    seq_count;

extern bool   save_eeprom;

extern bool   heat_enable;

extern uint8_t servonum;

#include <avr/wdt.h>                                                                   // Watchdog controller library
