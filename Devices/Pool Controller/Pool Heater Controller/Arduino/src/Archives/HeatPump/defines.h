// Define returned error codes
#define ERROR_UNDEFINED_COMMAND 9999                                                   //Reply "9999" means the command is not valid
#define ERROR_COMMAND_NOT_ACTIVATED 9998                                               //Reply "9998" means the command is not activated (defined in current device)
#define ERROR_INVALID_DIGITAL_PIN 9997                                                 //Reply "9997" means the pin number is not valid for digital read/write
#define ERROR_INVALID_ANALOGIN_PIN 9996                                                //Reply "9996" means the pin number is not valid for analog input read
#define ERROR_INVALID_PWM_PIN 9995                                                     //Reply "9995" means the pin number is not valid for analog output write (PWM)
#define ERROR_SENSOR_READ 9992                                                         //Reply "9992" means the sensor is invalid or not found
#define ERROR_SENSOR_REQUEST_FAIL 9991                                                 //Reply "9991" mesan the sensor function received an invalid request

#define DISP_OFF_DELAY 60000
#define DISP_REF_DELAY 1000
#define DISP_CHG_DELAY 3500
#define DISP_HLD_DELAY 2000
#define FAN_OFF_DELAY 10000

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

//Static values
#define temp_lp_low 0
#define ratio_ax2 0.00007
#define ratio_bx 0.1153
#define ratio_c 4.2263

#define temp_min 75
#define temp_max 99

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



