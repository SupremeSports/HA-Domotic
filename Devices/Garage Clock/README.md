# Garage Clock #

#### Status: Parts purchased, code in development ####

### Description: ###
This digital wall clock for my garage is based on the TM1637 chip. The initial libraries TM1637.h and TM1637_6D.h were modified to adjust the number of digits per application.

### Features: ###
- Displays actual time (hh:mm:ss)
- Displays actual garage temperature/humidity (°C/°F) using a DHT22 since temperatures might get below 0°C/32°F
- Turns displays off when there's no one in the garage
- Auto dim with light intensity
