# Heaters and Thermostats

#### Main controller : ESP8266-01 and ESP8266-07 (depending on application)
#### Status: Some done, others for later

## Description:
The wall thermostat would be an ESP8266-07 or NodeMCU integrated with a LCD touchscreen (probably ArduiTouch). A DHT11 or DHT22 will be added to capture room's temperature and humidity. The thermostat will be communicating with the PLC to receive instructions or settings from HMIs or remote access, and will operate the "thermostat" function locally. Will emit a coil output signal on the Modbus.
The ConvectAir or heater will be equipped with an ESP8266-01 with a relay and will read thermostats single coil for ON or OFF.

#### Thermostats features:
- 2.4" TFT touchscreen
- DHT11/22 to read temperature and humidity
- One per room

#### Heaters features:
- Controls the heater to bypass the integrated one
- Allows control from remote locations.
- Readjust preset value

