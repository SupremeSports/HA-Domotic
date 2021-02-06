# Garage Main Controller

#### Main controller : Arduino Mega2560 with ENC28J60
#### Status: Partly in service

#### Features:
- Main contactors with Aux Contact feedback
  - Controls Heaters
  - Controls air compressor motor
- Reads indoor temperature/humidity in two places
  - Extra temperature safety monitoring (insure heaters are working)
- Controls outdoor lightings
- Monitors Garage alarm system
- Controls Garage door sensors and up/down/stop buttons

# Garage Slave Controller

#### Slave controller : Arduino Mega2560 with ENC28J60
#### Status: Partly in service

#### Features:
- Controls air compressor main valve (When there's no one in the garage, main valve closes to prevent leaks)
- Controls air compressor drain valve (water drain)
- Reads air compressor pressure
- Rear door sensor
