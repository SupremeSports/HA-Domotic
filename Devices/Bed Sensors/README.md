# BED PRESENCE DETECTION #

#### Status: Completed - In Service ####

#### Devices: ESP8266-07, HX711, 4x 50kg load cells ####

### Description: ###
I wanted a way to detect that we're going to bed in order to activate some automations and notifications. For example, let's say I forgot to lock the house door, close the garage door or lock car doors, all this could be automated through Home Assistant. But I needed a way to detect it. So I built a sensor using the HX711 and 50kg load cells to convert my bed in a giant scale. Ok, it's not exactly a scal as the sensor doesn't even convert it to a unit, I just use the values in order to set some ranges. Also, the 4 sensors are placed at the head of the bed, therefor, there's no weight repartition through the load cells, so they only get part of the down force. A tare setting is used to account for sheets change between winter and summer, etc.

### Features: ###
- Detects wife's presence, my presence or both
    - If the dog is alone on the bed, I can detect it.
- Tare function

#### Outcome ####
Device works very great

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- None
