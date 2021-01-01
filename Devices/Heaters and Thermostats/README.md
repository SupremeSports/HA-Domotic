# Heaters and Thermostats

#### Main controller : ESP8266-01, ESP8266-07 and ESP32 (depending on application)
#### Status: Some done, others for later

## Description:
The wall thermostats are controlled by ESP32-WROOM-32 integrated with a LCD touchscreen (based on AZ-Touch). An AM2320/DHT22 captures the room temperature and humidity. The thermostat is communicating with Home Assistant to receive/send settings through MQTT and operates the "thermostat" function locally.
The ConvectAir or heater will be equipped with an ESP8266-07 with a relay themperature sensor, etc...

#### Thermostats features:
- 2.4" TFT touchscreen
- AM2320/DHT22 to read temperature and humidity
- Auto dim when sun is down
- Auto screen turn off
- Settings screen to adjust TFT dim level, beeper level, screen off delay
- One per room

#### Heaters features:
- Controls the heater to bypass the integrated one
- Allows control from remote locations
- Readjust preset value
- Fail safe, is HA or thermostat fails, local integrated thermostat will function as expected
  - Will be adjusted lower than lowest programmed value but high enough to prevent freezing

