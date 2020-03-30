# Analog Wall Clock #

#### Status: Completed - In Service ####

#### Devices: ESP8266-07, A4988, A1302, steppers ####

### Description: ###
I was tired of seeing the clock out of time because it's large and hands are not well balanced. So batteries were going dead pretty quickly. Also, it is installed above high staircase and it's not very fun to re-adjust or replace battery. Of course, I could rebalance the hands, but where's the fun?

Based on 720 pulses per rotation stepper motors, the analog wall clock with hours and minutes hands (seconds hand is also programmed, but not physically installed) will adjust automatically to current time every 5 seconds and seconds hand every second (obviously?!). This implies the minutes hand has to go forward by 12 pulses every minute or once every 5 seconds. The hours hand will need to go forward by 60 pulses every hour or once every 5 minutes.

### Features: ###
- Dual-stepper analog clock with hour and minute hands
    - Possible to add second hand with almost no modifications to the code, but certain changes to 3D printed parts
- Magnetic hall sensors with tiny magnets (W:2mm x H:1mm) to detect hands at top (12 o'clock)
- Automatic homing on power up, leaves a bit of time to unplug if you need to install hands, then goes to actual time of day
- Automatically adjust minutes hand every hour if it missed many pulses during the hour
    - Would unfotunately consider it homed if few pulses out due to magnetic field (pretty much fixed with opamp)
- Automatically adjust hours hand every 12 hours (midnight and noon)
    - Would unfortunately consider it homed if few pulses out due to magnetic field (pretty much fixed with opamp)
- Automatically readjusts for DST (data comming from server)

***UPDATE:***
At first, I wanted to put the magnets at the end of the hands. Unfortunately, the shake too much (very small for your eye, but too much for the sensor) to be able to get a correct reading and prevent hand from going too far. So, I instead added a LM393 opamp on the signal wires from the hall sensors giving me very precise trigger; it is very difficult to adjust the potentiometer (sometimes too much or not enough) but once correct, it works very well.

#### Outcome ####
Device works great with the opamp in place.
A real PCB will be built to replace the breadboard and make things nicer and safer.
Will probably reprint the 3D parts to get it slimmer (later).

### Home Assistant Link: ###
- MQTT device with QoS and LWT
- Receives current date/time from the HA server
    - Automatically requests (e.g. on startup) time
- DST time sent by server replicates on clock immediately
- User can trigger an homing request in case the clock is out of time

#### Credits & 3D printing
- Gears and others: [SupremeSports](https://www.thingiverse.com/thing:3768567)
