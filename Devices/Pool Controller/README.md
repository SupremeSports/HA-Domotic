# Pool controller:

#### Main controller :  Arduino Nano with ENC28J60
#### Status: Up and running (Modbus version)

#### Features:
- Controls pool pump dual speeds depending on time of day
- Controls heat pump depending on time of day and temperature setpoint
- Reads water level and controls automatic refill
- Control water circulation valves

## In Development or future

#### Status: Parts purchased, code in development
#### Features in development:
- Controls circulation valves to choose between filter, drain.
- Controls return flow to choose between solar panels, heat pump or bypass
  - Reads water temperature at each step and a PID controller tries to get the best heating/$ ratio
  
#### Features for future:
- Read chlorine and pH level (eventually, will automatically control the levels)
- Upgrade to MQTT with an Arduino Mega2560
