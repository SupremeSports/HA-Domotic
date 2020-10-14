# HVAC - Living Room #

#### Status: Completed - In Service ####

#### Devices: ESP8266-07, HLK-PM03, 10k NTC ####

### Description: ###
My HVAC remote control is always out of hand and often misplaced, also I wanted to be able to control temperature from HA as well as automating everyheater/HVAC into a single ecosystem. This will allow to eventually go forward with peak period offsets with the utility company in order to save couple hundred bucks per year...

![alt text](images/AddedControl.jpg "Control Board")

![alt text](images/IRdiode.jpg "IR LED setup")

![alt text](images/TempSencor.jpg "Temperature Sensor")

### Features: ###
- HLK-PM03 3.3V 1W power supply directly on 240Vac
- 940nm IR LED to communicate as the original remote
- Extra temp sensor to read the output temperature (NOT room temperature)
- Fan sensor to know when it runs as there's no feedback back to remote, so no feedback to this module
- Special library for Fujitsu AC units has been used
    - Will need to be adapted to your own AC unit
    - **Unless you have the EXACT same unit and especially the same remote as I do, it WON'T work**

#### Outcome ####
Device works very great
A real PCB will maybe be built to replace bulky wires and make things nicer and safer.

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- IR Remotes Library: [crankyoldgit](https://github.com/crankyoldgit/IRremoteESP8266)
