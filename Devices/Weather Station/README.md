# Weather Station

#### Main controller : Arduino Mega 2560 with ENC28J60 and POE ####
#### Status: Completed - In Service ####

### Description: ###
I wanted my own personnal data for my location. So, I decided to build my own weatehr station with as much sensors as possible.

#### Features: ####
- Reads Air Quality Index - MQ135
- Reads solar UV level - VEML6070 
- Reads outdoor temperature - AM2320 & BME280
- Reads outdoor humidity - AM2320 & BME280
- Reads outdoor atmospheric pressure - BME280
- Reads wind speed - Using 1 photodiode & IR emitter and a disc with 4 slots (Might increase for better precision)
- Reads wind direction - Using 5 photodiodes & IR emitters and a Single-Track Gray-Coded disc
- Rain level - Using 1 photodiode & IR emitter with a tilting bucket
- Rain sensor - Using an eBay type simple rain sensor with op-amp analog circuit
    - This sensor eneded up corroded and will need to find something else (optical?)

- Reads internal temperature/humidity - AM2320
- Reads input and 5V rail voltages

#### Features for future: ####
- Lightning Detection
- Replace analog rain sensor with an optical rain detection

### Home Assistant Link: ###
- MQTT device with QoS and LWT
- Receives all data once per second
- Calculates wind chill or heat index depending on temperature
- Calculates UV index
- Adjusts icon's colors depending on current status
- Some sensors has custom images such as the wind direction (one wind rose icon for each direction - 32 steps)

#### Credits & 3D printing
- Anemometer:   [Yoctopuce](https://www.thingiverse.com/thing:41367)
- Wind vane:    [Yoctopuce](https://www.thingiverse.com/thing:42858)
- Rain level:   [Yoctopuce](https://www.thingiverse.com/thing:119653)
- Solar Shield: [SupremeSports](https://www.thingiverse.com/thing:3702208)
- Control Box:  [SupremeSports](https://www.thingiverse.com/thing:3702240)
