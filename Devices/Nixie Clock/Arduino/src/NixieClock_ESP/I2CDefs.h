// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- I2C DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#ifndef I2CDefs_h
#define I2CDefs_h

// I2C Interface definition
#define I2C_SLAVE_ADDR                 0x69
#define I2C_TIME_UPDATE                0x00
#define I2C_GET_OPTIONS                0x01
#define I2C_SET_OPTION_12_24           0x02
#define I2C_SET_OPTION_BLANK_LEAD      0x03
#define I2C_SET_OPTION_SCROLLBACK      0x04
#define I2C_SET_OPTION_SUPPRESS_ACP    0x05
#define I2C_SET_OPTION_DATE_FORMAT     0x06
#define I2C_SET_OPTION_DAY_BLANKING    0x07
#define I2C_SET_OPTION_BLANK_START     0x08
#define I2C_SET_OPTION_BLANK_END       0x09
#define I2C_SET_OPTION_FADE_STEPS      0x0a
#define I2C_SET_OPTION_SCROLL_STEPS    0x0b
#define I2C_SET_OPTION_BACKLIGHT_MODE  0x0c
#define I2C_SET_OPTION_RED_CHANNEL     0x0d
#define I2C_SET_OPTION_GREEN_CHANNEL   0x0e
#define I2C_SET_OPTION_BLUE_CHANNEL    0x0f
#define I2C_SET_OPTION_CYCLE_SPEED     0x10
#define I2C_SHOW_IP_ADDR               0x11
#define I2C_SET_OPTION_FADE            0x12
#define I2C_SET_OPTION_USE_LDR         0x13
#define I2C_SET_OPTION_BLANK_MODE      0x14
#define I2C_SET_OPTION_SLOTS_MODE      0x15
#define I2C_SET_OPTION_MIN_DIM         0x16

#define I2C_DATA_SIZE                  22
#define I2C_PROTOCOL_NUMBER            54

#endif

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- I2C LIMITS DEFINES ----------------------------------------
// ----------------------------------------------------------------------------------------------------

//Defines defaults, min and max according to the user manual available on 
//https://www.nixieclock.biz/Downloads/NixieClockUserManualV52.pdf

//Boolean values 0 or 1
#define HOUR_MODE_DEFAULT               0   //24h
#define LEAD_BLANK_DEFAULT              0   //Don't blank
#define SCROLLBACK_DEFAULT              1   //Enabled
#define SUPPRESS_ACP_DEFAULT            0   //Always do ACP
#define FADE_DEFAULT                    1   //Enabled
#define USE_LDR_DEFAULT                 1   //Enabled
#define SLOTS_MODE_DEFAULT              1   //Enabled

//Other analog values
#define DATE_FORMAT_DEFAULT             2   //DD-MM-YY
#define DATE_FORMAT_MIN                 0
#define DATE_FORMAT_MAX                 2

#define DAY_BLANKING_DEFAULT            0   //Don't blank
#define DAY_BLANKING_MIN                0
#define DAY_BLANKING_MAX                8

#define BLANK_FROM_DEFAULT              0   //00h
#define BLANK_FROM_MIN                  0
#define BLANK_FROM_MAX                  23

#define BLANK_TO_DEFAULT                7   //07h
#define BLANK_TO_MIN                    0
#define BLANK_TO_MAX                    23

#define FADE_STEPS_DEFAULT              50  //The number of display impessions we need to fade by default (100 is about 1 second)
#define FADE_STEPS_MIN                  20
#define FADE_STEPS_MAX                  200

#define SCROLL_STEPS_DEFAULT            4   //How quickly the scroll works
#define SCROLL_STEPS_MIN                1
#define SCROLL_STEPS_MAX                40

#define BACKLIGHT_DEFAULT               0   //Fixed
#define BACKLIGHT_MIN                   0
#define BACKLIGHT_MAX                   5

#define COLOUR_RED_CNL_DEFAULT          15  //Full RED
#define COLOUR_GRN_CNL_DEFAULT          15  //Full GREEN
#define COLOUR_BLU_CNL_DEFAULT          15  //Full BLUE
#define COLOUR_CNL_MIN                  0
#define COLOUR_CNL_MAX                  15

#define CYCLE_SPEED_DEFAULT             10  //The higher the number, the slower the colors will change
#define CYCLE_SPEED_MIN                 4
#define CYCLE_SPEED_MAX                 64

#define BLANK_MODE_DEFAULT              2   //Tubes and LEDs blanked
#define BLANK_MODE_MIN                  0
#define BLANK_MODE_MAX                  2

#define MIN_DIM_DEFAULT                 100  //Minimum brightness wanted
#define MIN_DIM_MIN                     100  //The minimum dim count
#define MIN_DIM_MAX                     500  //The maximum dim count
