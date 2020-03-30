//PINOUT:
//SDA = A4
//SCL = A5
#include "Arduino.h"
#include "OLED.h"

#include <SSD1306_text.h>

#include <Wire.h>

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C  // 011110+SA0+RW - 0x3C or 0x3D

#define OLED_RESET 22     // Reset pin or 22 for no reset pin
SSD1306_text display(OLED_RESET);

oledclass::oledclass(){}

void oledclass::SET()
{
  display.init(I2C_ADDRESS);
  display.clear();                        // clear screen
}

void oledclass::CLEAR()
{
  display.clear();                        // clear screen
}

void oledclass::CHAR(int x, int y, String sym, int fontsize, bool clearline)
{
  display.setTextSize(fontsize, 1);       // Character size, spacing in pixels

  display.setCursor(y, x);
  
  if(clearline)
    display.clearToEOL();
  
  //display.setCursor(y, x);
  display.write(sym.c_str());
}

oledclass OLED = oledclass();
