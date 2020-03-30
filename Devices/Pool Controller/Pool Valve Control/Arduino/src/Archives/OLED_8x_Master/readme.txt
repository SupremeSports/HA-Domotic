This is a text-only library for the SSD1306 128x64 display.
It is only ported to the Arduino Uno R3.

This library uses no screen buffer. It writes directly to the display.
Thus it is most efficient to write static elements once and then overwrite
existing characters rather than clear the screen and rewrite everything.

The library supports H/W SPI, soft SPI and I2C.


Methods
-------
init() - call once during setup
clear() - clears the display
setCursor(row, col) - row: 0-7, col: 0-127
setTextSize(size, spacing) - size: 1-8; spacing: 0-N
write(c) - draw a single character, like 'B'
write(s) - draw a string
writeInt(i) - draw an integer

print() is supported unless disabled in the .h file (saves a little code space)


