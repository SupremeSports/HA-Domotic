# Thermostat - Living Room #

#### Status: In developement ####

#### Devices: ESP8266-07, HLK-PM03, DHT22, AZ-Touch####

### Description: ###
Needed a thermostat to read room temperature in order to control the HVAC accordingly.
The 2.4" TFT touchscreen replaces the HVAC remote control and communicates with [Living HVAC](https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Heaters%20and%20Thermostats/Living%20HVAC).
It also replicates [Home Assistant climate](https://www.home-assistant.io/integrations/climate/) options onto the screen.
The basics is from [AZ-Touch](https://www.hwhardsoft.de/english/projects/arduitouch-esp/) and I simply used parts of their code in order to make mine. Many features added, some others removed.

Images to come...
<!-- ![alt text](images/AddedControl.jpg "Control Board")

![alt text](images/IRdiode.jpg "IR LED setup")

![alt text](images/TempSencor.jpg "Temperature Sensor") -->

### Features: ###
- HLK-PM03 3.3V 1W power supply directly on 120Vac
- Password protected settings and temperature modification (disables buttons)
- Password can be changed through MQTT and saved to EEPROM
- Logout and screen timeout adjustable through the settings screen
- Extra screen to show information (WiFi SSID, rssi, voltage, etc.)
- Communicated with Home assistant AND HVAC directly through the MQTT browser

- Top row 
    - Displays WiFi status, HVAC on/off status, ECO mode, user logged in and notifications icons
    - Displays time
- More to come...

#### Outcome ####
Bench tests are very impressive, waiting for my order to come in (not ordered yet :'( )
To come...

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- Basic code and case: [AZ-Touch](https://www.hwhardsoft.de/english/projects/arduitouch-esp/)
