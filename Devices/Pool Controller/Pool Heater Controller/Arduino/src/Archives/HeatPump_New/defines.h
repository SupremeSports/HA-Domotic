#define OLED_ENABLE

//Inputs/Outputs pinout
//Push buttons - Controlled by voltage divider
#define BUTTONS 14

//Heater analog sensors
#define TEMP_LP 14
#define TEMP_IN_WATER 20
#define TEMP_OUT_WATER 21

//Heater digital sensors
#define SWITCH_FLOW 3 
#define SWITCH_HP 7
#define SWITCH_LP 8

//Heater digital outputs
#define RELAY_FAN 9
#define RELAY_HEAT 6

#define TEMP_MIN 75
#define TEMP_MAX 99

//Static values
#define temp_lp_low 0
#define ratio_ax2 0.00007
#define ratio_bx 0.1153
#define ratio_c 4.2263

#define TEMP_MIN 75
#define TEMP_MAX 99

//Up button nominal value should be 170
#define Up_min 165 //analog value from 0-1023
#define Up_max 175 //analog value from 0-1023

//Down button nominal value should be 202
#define Down_min 197 //analog value from 0-1023
#define Down_max 207 //analog value from 0-1023

//Lock button nominal value should be 254
#define Lock_min 249 //analog value from 0-1023
#define Lock_max 259 //analog value from 0-1023

//POOL/SPA button nominal value should be 339
#define Spa_min 334 //analog value from 0-1023
#define Spa_max 344 //analog value from 0-1023

#define HOLD 1
