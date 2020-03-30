# TANKLESS WATER HEATER #

#### Status: Completed - In Service ####

#### Devices: ESP8266-07, 10k NTC ####

### Description: ###
IMPRTANT - THIS PROJECT IS DANGEROUS: Playing with Mains or Line voltage is always dangerous. Please proceed with caution. This writer assumes no responsibility if you die or get shocked or any other damage you may create by playing where you shouldn't.

IMPORTANT - NORMAL WATER HEATER: While this could be achieved with a normal water heater, extra contactors or triac circuits will be needed in order to control each element independently. A flow sensor would need to be added and some temperature sensors. I STRONGLY recommand not disabling original thermostat and keeping it to a safe level (52-60°C or anything as per your local codes). You would then be able to stop water heating during peak periods and save money on your electricity bill. On anormal water heater, you should have plenty of hot water for an hour or two without heating. You keep the pilot heater (1kW) and shut the other 2kW(<40gal) or 2x2kW(40-60gal).

IMPORTANT - TANKLESS WATER HEATER: This project as been conducted on a JNOD XFJ270FDCH (27kW).

So, this heater needs water flow to work safely and could get ON for no reason while water supply is off. So with my valve controller([see home control](https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Control%20Room)), when I shut water valve off (e.g. while not at home for extended periods), the water heater will also be shut off.

So the principle is very simple, this tankless water heater uses 3 sets of triacs driven by MOC3021 to activate 3x 9kW heaters. So I added a 2N3906 on the 5V+ line going to the MOC3021 control side (cut PCB trace going to LEDs). This transistor is controlled through a NFET to trigger from the ESP8266.

While I was there, I also added a 10k NTC circuit to read output temperature (code and board ready to read input temperature). I then tapped into the flow sensor. This will allow, eventually, to pop an alarm if water flows for too long. Another flow sensor will be added on cold water line to eventually shut the valve if it flows for more than, let's say, 20 minutes in a row (preventing water spills in case of broken pipe).

### Features: ###
- Enable/Disable water heater
- Read output temperature (slow response as NTC is not into the pipe, will maybe drill it later)
- Read flow sensor 
    - Home made code for the ESP8266 as there's no good non-blocking frequency readinglibrary

#### Outcome ####
Device works very great

### Home Assistant Link: ###
- MQTT device with QoS and LWT

#### Credits & 3D printing
- None
