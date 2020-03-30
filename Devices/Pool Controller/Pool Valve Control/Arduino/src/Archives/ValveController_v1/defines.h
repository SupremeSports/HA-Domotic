#define DEBUG_ENABLE

#define OLED_ENABLE
#define ETHERNET_ENABLE
#define EEPROM_ENABLED

//Inputs/Outputs pinout on Arduino
#define MUX_S0 3
#define MUX_S1 4
#define MUX_S2 5
#define MUX_S3 6
#define MUX_SIG 14 //A0

#define BUTTON_UP 7
#define BUTTON_DN 8
#define BUTTON_LK 9

//I2C Signals mapping
#define Mtr_1 0
#define Mtr_2 1
#define Mtr_3 2
#define Dsp_1 5
#define Dsp_2 6
#define Dsp_3 7

//Inputs pinout on Analog MUX CD74HC4067
//Temperature sensors
#define Temp_Water_Inlet  0   // Water Temperature Inlet after filter
#define Temp_Water_Solar  1   // Water Temperature Solar after panels
#define Temp_Water_Mid    2   // Water Temperature between solar and heat pump
#define Temp_Water_Heater 3   // Water Temperature after heatpump
#define Temp_Water_Return 4   // Water Temperature after everything and just before return
#define Temp_Roof_Surface 5   // Roof Temperature

//I2C pinout on I2C MUX TCA9548A 
#define TCAADDR 0x70
//I2C Bus Pinout
#define Disp_Solar        0
#define Disp_Heater       1
#define Disp_Mixer        2
#define Motors_Board      7

//Outputs pinout on Adafruit 16-Channel Servo Driver (PCA9685)
#define PWM_MTR1          0
#define PWM_MTR2          1
#define PWM_MTR3          2

//Control potentiometers
#define POT_Solar         6
#define POT_Heater        7
#define POT_Inlet         8

//Others
#define Sensor_Sun        9
#define Sensor_Rain       10
#define Sensor_Pressure   11


#define SOLAR_MIN         250 //0-1023

#define MIN_PULSE_WIDTH       650
#define MAX_PULSE_WIDTH       2350
#define DEFAULT_PULSE_WIDTH   1500
#define FREQUENCY             60

#define temp_ratio_ax2    0.00007
#define temp_ratio_bx     0.1153
#define temp_ratio_c      4.2263

#define pres_ratio_ax2    0.0000
#define pres_ratio_bx     0.2444
#define pres_ratio_c    -25.0000

#define sun_ratio_ax2     0.0000
#define sun_ratio_bx      1.0000
#define sun_ratio_c       0.0000

#define calc_solar_ax2    0.0000
#define calc_solar_bx     3.3300
#define calc_solar_c     -0.2000

#define heatpump_min      70
#define antifreeze        40

#define DISP_BASE_NOW     0
#define DISP_RUN          1
#define DISP_MAX          625

#define TEST_OLED         0

#define DISP_OFF_DELAY 60000  //ms

#define TEMP_MIN 74
#define TEMP_MAX 99
#define TEMP_DEF 85

#define DISP_HOLD 1000  //ms
#define LOOP_TIME 100   //ms
#define LOOP_HOLD 30    //Hold cycle counts
