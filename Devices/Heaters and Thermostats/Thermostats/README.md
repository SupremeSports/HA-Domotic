# Thermostat - Living Room #

#### Status: In developement ####

#### Devices: ESP8266-07, HLK-PM03, DHT22, AZ-Touch ####

### Description: ###
Needed a thermostat to read room temperature in order to control the HVAC/Heaters accordingly.
The 2.4" TFT touchscreen replaces the HVAC remote control and communicates with my [Living HVAC](https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Heaters%20and%20Thermostats/Living%20HVAC).
The 2.4" TFT touchscreen will be eventually deployed to all my rooms to get temp/humidity and eventually control heaters.
It also replicates [Home Assistant climate](https://www.home-assistant.io/integrations/climate/) options onto the screen.
The basics is from [AZ-Touch](https://www.hwhardsoft.de/english/projects/arduitouch-esp/) and I simply used parts of their code in order to make mine. Many features added, some others removed.

Pictures/screenshots from TFT to come...
[alt text](images/FrontView.jpg "Front View")[alt text](images/BackView.jpg "Back View")
<!-- ![alt text](images/AddedControl.jpg "Control Board")

![alt text](images/IRdiode.jpg "IR LED setup")

![alt text](images/TempSencor.jpg "Temperature Sensor") -->

### Features: ###
- HLK-PM03 3.3V 3W power supply directly on 120Vac
- Passcode protected settings and temperature modification (disables and/or hides buttons)
- Passcode can be changed through MQTT and saved to EEPROM
- Logout and screen timeout adjustable through the settings screen
- Settings screen to adjust screen turn off delay, logout delay, dim level, beeper level
- Extra screen to show information (WiFi SSID, rssi, voltage, etc.)
- Communicating with Home Assistant and HVAC directly through the MQTT broker

- Top row:
    - Displays WiFi status, HVAC on/off status, ECO mode, user logged in and notifications icons
    - Displays time
- More to come...




#### Outcome ####
Bench tests are very satisfying, first prototype working, waiting on few more parts to come in mail...

To come...

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- Basic code: [AZ-Touch](https://www.hwhardsoft.de/english/projects/arduitouch-esp/)
- 3D printed case : [SupremeSports](https://www.thingiverse.com/supremesports/designs) - To come...
