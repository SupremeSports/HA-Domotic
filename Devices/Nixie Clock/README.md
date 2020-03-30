# Nixie Clock #

#### Status: Completed - In Service ####

#### Devices: ESP8266-01, ATMEGA328P, K155ID1, Nixie IN-14 Tubes ####

### Description: ###
I wanted a vintage clock for my retro-industrial bar/mancave at home. What better than a nixie clock? My project is mostly based on [NixieClock.biz](https://nixieclock.biz) project and I modified it to connect to my MQTT HomeAssistant and runs on internal RTC most of the time. ESP-01 syncs with HA once every 5 minutes, updates clock through I2C once per hour.

I purchased a kit from NixieClock.biz since it was cheaper than trying to build my own. I already built one back in 2005 for a school project; it was using the 60Hz from the outlet to pulse the clock. Was working great, but was sometimes missing pulses. I always wanted to rebuild one for me.

### Arduino Uno main clock code ###
Original Code Repository Clock: [Firmware V1 6 Digit Code](https://bitbucket.org/isparkes/nixiefirmwarev1/src/master/ardunixFade9_6_digit/)

The main code is almost exactly the one provided by [NixieClock.biz](https://www.nixieclock.biz/Store.html). The code has been modified to turn the board upside down and install the bulbs underneath. Luckily, the anode pin is in the middle and "mirroring" the tubes is not affecting anything more than digits pinout. Other than that, it has not been modified and could be used for any other Rev2 and higher boards:

    //If you mount the PCB upside down, pins need to be reassigned (Simply change this define)
    #define AIO_UPSIDEDOWN //[AIO_UPSIDEDOWN,NOT_AIO_UPSIDEDOWN] - Exclusively for upside down tubes (PCB mounted upside down)
    
    // Used for special mappings of the K155ID1 -> digit (wiring aid)
    // Allows the board wiring to be much simpler
    #ifndef AIO_UPSIDEDOWN
      byte decodeDigit[16]    = {2,3,7,6,4,5,1,0,9,8,10,10,10,10,10,10};  // Standard mounting PCB
      byte decodePosition[6]  = {5,4,3,2,1,0};                            // Standard mounting PCB
    #else
      byte decodeDigit[16]    = {3,2,8,9,0,1,5,4,6,7,10,10,10,10,10,10};  // Upside down mounting PCB
      byte decodePosition[6]  = {0,1,2,3,4,5};                            // Upside down mounting PCB
    #endif

    // Anode pins
    // These are now managed directly on PORT C and PORT D, we don't use digitalWrite() for these
    #ifndef AIO_UPSIDEDOWN
      #define ledPin_a_6                    0     // Low  - Secs  units // package pin 2  // PD0
      #define ledPin_a_5                    1     //      - Secs  tens  // package pin 3  // PD1
      #define ledPin_a_4                    2     //      - Mins  units // package pin 4  // PD2
      #define ledPin_a_3                    4     //      - Mins  tens  // package pin 6  // PD4
      #define ledPin_a_2                    A2    //      - Hours units // package pin 25 // PC2
      #define ledPin_a_1                    A3    // High - Hours tens  // package pin 26 // PC3
    #else
      #define ledPin_a_6                    A3    // High - Hours tens  // package pin 26 // PC3
      #define ledPin_a_5                    A2    //      - Hours units // package pin 25 // PC2
      #define ledPin_a_4                    4     //      - Mins  tens  // package pin 6  // PD4
      #define ledPin_a_3                    2     //      - Mins  units // package pin 4  // PD2
      #define ledPin_a_2                    1     //      - Secs  tens  // package pin 3  // PD1
      #define ledPin_a_1                    0     // Low  - Secs  units // package pin 2  // PD0
    #endif

It includes many cool features (anti-ghosting, fading, colored LEDs) along with very important features such as anti-cathode-poisoning, tubes healing etc. Therefor, it's very interesting to keep the provided code.

### ESP8266-01 Time provider ###
Original Code Repository Wifi: [Firmware V1 Wifi](https://bitbucket.org/isparkes/nixiefirmwarev1/src/master/WifiTimeProviderESP8266/)

The "All-in-one" arduino module from NixieClock.biz has a port to plug and ESP8266-01 and has an I2C port programmed to send/receive commands and receive time updates. The module is running [NixieClock.biz](https://www.nixieclock.biz/Store.html) provided code which is supposed to access the internet to read the time; I greatly modified the code to connect it to my HA instead. Since everything on my home automation is on a secured network, I don't want to give internet access to devices; they will receive the time from the HA through MQTT.

For the settings, twenty are defined by default from the original code. Even though I could have eliminated some and hard code them, I decided to keep them all simply because, well... more options is cool.

### Features ###
- Auto time adjust from server
- The options shown in the manual are controlled from HA instead of the integrated web server
- Values limited as per user manual
- Wifi Interface for an ESP8266
- Real Time Clock interface for DS3231 - Not used in my case, but could be added
- Digit fading with configurable fade length
- Digit scrollback with configurable scroll speed
- Configuration stored in EEPROM and sent to HA
- Low hardware component count (as much as possible done in code)
- Single button operation with software debounce
- Single K155ID1 for digit display (other versions use 2 or even 6)
- RGB Back lighting
- Automatic linear dimming using light sensor

#### Outcome ####
Device works great

### Home Assistant Link: ###
- MQTT device with QoS and LWT
- Receives current date/time from the HA server
    - Automatically requests (e.g. on startup) time
- DST time sent by server replicates on clock immediately
- Options are read from the Arduino through I2C to the ESP01 then to HA
- Options sent from HA to the ESP01 is sent through I2C to arduino and saved to EEPROM
- User can turn on/off the clock, the RGB LEDs, change color and change features of the clock from HA

#### Credits & 3D printing
- [NixieClock.biz](https://nixieclock.biz)
