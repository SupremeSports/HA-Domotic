#define DEBUG_ENABLE

#define OLED_ENABLE
#define ETHERNET_ENABLE
#define EEPROM_ENABLED

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
#define RELAY_FAN_ON 9      //Pin D9 - Digital in
#define RELAY_FAN_HIGH 5    //Pin D5 - Digital in
#define RELAY_HEAT 6        //Pin D6 - Digital in

//Spares
#define SPARE_D4 4

#define SPARE_A2 16
#define SPARE_A3 17
#define SPARE_A4 18

//Dont use following pins
//SPI for ethernet      *pins may be used if ethernet not used and NOT conencted
//10, 11, 12, 13
//I2C for OLED display  *pins may be used if I2C not used and NOT conencted
//18, 19

//Static values
#define temp_lp_low 500
#define ratio_ax2 0.00007
#define ratio_bx 0.1153
#define ratio_c 4.2263

#define TEMP_MIN 69
#define TEMP_MAX 99
#define TEMP_DEF 85

#define FAN_OFF_DELAY 30000
#define DISP_OFF_DELAY 10000

//Up button nominal value should be 170
#define UP_MIN 165 //analog value from 0-1023
#define UP_MAX 175 //analog value from 0-1023

//Down button nominal value should be 202
#define DN_MIN 197 //analog value from 0-1023
#define DN_MAX 207 //analog value from 0-1023

//Lock button nominal value should be 254
#define LK_MIN 249 //analog value from 0-1023
#define LK_MAX 259 //analog value from 0-1023

//POOL/SPA mode button nominal value should be 339
#define MD_MIN 334 //analog value from 0-1023
#define MD_MAX 344 //analog value from 0-1023

#define DISP_HOLD 1000  //ms
#define LOOP_TIME 100   //ms
#define LOOP_HOLD 10    //Hold cycle counts
#define SAVE_DELAY 2    //Normal display cycle counts

#define DISP_BASE_NOW 0
#define DISP_RUN 1
#define DISP_MAX 2501


