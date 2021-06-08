# Maestro-Dual-Dimmer #
## Revisited version of Maestro Light/Fan Dimmers ##

#### Status: In final testings ####

#### Devices: ESP32, NRF24L01, Atmega328PB (Nano), PCF9685 ####

### Description: ###
The original Maestro switches I had were not working anymore. They use X10 communication on zerocrossing.
I had three of them and they were cross talking and two of them finally stopped working completely. Since I do like really much the design of the switches, I decided to recreate the code and PCBs with same features. I also added a Wifi feature to include it in Home Assistant.

This repository will include codes, schematics and PCB files

### Features: ###
#### Wall Controller ####
Original: Maestro MA-LFQ35 Light/Fan

The switch module is an Atmega328PB matched with a NRF24L01 and PCF9685.

- Up and Down buttons to control dim level from 1-7
- Power button to set ON or OFF
    - Double click, sets brightness/speed to maxiumum
    - Double-click again, returns to preset level
- EEPROM to save settings and everything is saved in the switch module
- PWM LED controller to adjust brightness of each LED (PCF9685)

#### Canopy Controller ####
Original: Maestro CM-L300FQ1 Light/Fan

The canopy module uses ESP32 as main controller matched with a NRF24L01 to send/receive data to/from wall switch. The thyristors control is done using a dedicated Atmega328PB with zero-cross detection and MOC3021 to trigger the BTA16 thyristors.

- The ESP32 is able to run with or without WiFi and works as full standalone
- Communication through I2C to the Atmega328PB
- Fade in/out from one level to the other (optional through MQTT and saved to EEPROM)

#### Outcome ####
Bench tests are very impressive, first prototype installed and testing in progress with close monitoring as fire hazards are pretty high.

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- Dimmer control: Revisited [Dimmable-Light-Arduino](https://github.com/fabiuz7/Dimmable-Light-Arduino)
    - Use my revisited version: [SupremeSports_Dimmer](https://github.com/SupremeSports/Dimmable-Light-Arduino)
- Button switch: [arduino-multi-button](https://github.com/poelstra/arduino-multi-button)

More data and codes to come...
