#define DEBUG_ENABLE

#define OLED_ENABLE
//#define ETHERNET_ENABLE
//#define EEPROM_ENABLED

//Inputs/Outputs pinout

//Push buttons - Controlled by voltage divider
#define BUTTONS 14          //Pin A0 - Analog in

//Analog sensors
#define TEMP_LP 15          //Pin A1 - Analog in
#define TEMP_IN_WATER 20    //Pin A6 - Analog in
#define TEMP_OUT_WATER 21   //Pin A7 - Analog in

//Digital sensors
#define SWITCH_FLOW 3       //Pin D3 - Digital in
#define SWITCH_HP 7         //Pin D7 - Digital in
#define SWITCH_LP 8         //Pin D8 - Digital in

//Digital outputs
#define RELAY_FAN 9         //Pin D9 - Digital in
#define RELAY_HEAT_HIGH 5    //Pin D5 - Digital in
#define RELAY_HEAT_ON 6   //Pin D6 - Digital in

//Spares
#define SPARE_D4 4

#define SPARE_A2 16
#define SPARE_A3 17
#define SPARE_A4 18

//Dont use following pins
//SPI for etyhernet
//10, 11, 12, 13
//I2C for OLED display
//18, 19

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

#define DISP_HOLD 1
#define DISP_HEAT 5
#define LOOP_TIME 500 //ms
#define LOOP_HOLD 4
#define SAVE_DELAY 2

#define DISP_BASE_DELAY_LONG 91
#define DISP_BASE_DELAY_SHRT 93
#define DISP_BASE_NOW 95
#define DISP_RUN 96
#define DISP_MAX 100



