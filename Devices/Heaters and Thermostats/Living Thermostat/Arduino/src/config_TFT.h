/*______Define LCD pins for ArduiTouch _______*/
#ifdef ESP32
  #define TFT_CS        5
  #define TFT_DC        4
  #define TFT_MOSI      23
  #define TFT_CLK       18
  #define TFT_RST       22
  #define TFT_MISO      19
  #define TFT_LED       15
  #define TFT_LED_HIGH        //LED active when high, if active on LOW, disable this line

  #define TFT_LED_FREQ  5000
  #define TFT_LED_RES   8
  #define TFT_LED_CH    1
#elif ESP8266
  #define TFT_CS        5 //D1
  #define TFT_DC        4 //D2
  //#define TFT_MOSI    13
  //#define TFT_CLK     14
  //#define TFT_RST     N/A
  //#define TFT_MISO    12
  #define TFT_LED       15
  #define TFT_LED_HIGH        //LED active when high, if active on LOW, disable this line
#endif

#define UPSIDE_DOWN

#define HAVE_TOUCHPAD
#ifdef ESP32
  #define TOUCH_CS 14
  #define TOUCH_IRQ 16    //Was GPIO2
#elif ESP8266
  #define TOUCH_CS 0
  #define TOUCH_IRQ 2
#endif
/*_______End of defanitions______*/


/*______Assign pressure_______*/
#define ILI9341_ULTRA_DARKGREY    0x632C      
#define MINPRESSURE 10
#define MAXPRESSURE 2000
/*_______Assigned______*/

/*____Calibrate TFT LCD_____*/
#define TS_MINX 370
#define TS_MINY 470
#define TS_MAXX 3700
#define TS_MAXY 3600
/*______End of Calibration______*/


/*____Program specific constants_____*/
#define MAX_TEMPERATURE 30  
#define MIN_TEMPERATURE 16
#define DEF_TEMPERATURE 20
enum { INIT, PM_BOOT, PM_MAIN, PM_OPTION, PM_CLEANING, PM_CODE, PM_SLEEP };  // Program modes
enum { BOOT, COOLING, TEMP_OK, HEATING };                                    // Thermostat modes

#ifdef HAVE_TOUCHPAD
  #define ENABLE_BEEPER
#endif
#ifdef ESP32
  #define BEEPER_PIN    21
  #define BEEPER_FREQ   2700
  #define BEEPER_RES    12
  #define BEEPER_CH     5
#elif ESP8266
  #define BEEPER_PIN    16
#endif

#define OPTION1 true
#define OPTION2 true
#define OPTION3 true
#define OPTION4 true
#define OPTION5 true
#define OPTION6 false
#define OPTION7 false
#define OPTION8 false
#define OPTION9 false
#define OPTION10 true
#define MAXPAGE 4 //3 options per page
/*______End of specific constants______*/

// Color definitions
#define ILI9341_BLACK         0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY          0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN     0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN      0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON        0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE        0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE         0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY     0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY      0x7BEF      /* 128, 128, 128 */
#define ILI9341_VERYDARKGREY  0x632C      /* 100, 100, 100 */
#define ILI9341_BLUE          0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN         0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN          0x07FF      /*   0, 255, 255 */
#define ILI9341_RED           0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA       0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW        0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE         0xFFFF      /* 255, 255, 255 */
#define ILI9341_LIGHTBLUE     0x4A7F      /* 77,   77, 255 */
//#define ILI9341_ORANGE        0xFD20      /* 255, 165,   0 */ //LightOrange
#define ILI9341_ORANGE        0xEB63      /* 242, 112,   28 */  //RedOrange
//#define ILI9341_ORANGE        0xFC40      /* 255, 140,   0 */ //DarkOrange
#define ILI9341_GREENYELLOW   0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK          0xF81F

#define ILI9341_COLOR ILI9341_BLUE //Choose a color from above (e.g.: ILI9341_BLUE) or put 0 to disable
