//PINOUT:
//SDA = A4
//SCL = A5
#include "Arduino.h"
#include "OLED.h"

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

SSD1306AsciiWire disp;

oledclass::oledclass(){}

bool oledclass::SET()
{
  Wire.begin();         
  disp.begin(&Adafruit128x64, I2C_ADDRESS);
  disp.set400kHz();
  return(true);
}

bool oledclass::DISPON()
{
  SSD1306_DISPLAYON;
  return(true);
}

bool oledclass::DISPOFF()
{
  SSD1306_DISPLAYOFF;
  //return(false);
  return(true);
}

void oledclass::CLEAR()
{
  disp.clear();
}

void oledclass::CHAR(int x, int y, String sym, int fontsize, bool clearline)
{
  disp.setFont(Adafruit5x7);  

  disp.setCursor(x,y);

  if(clearline)
    disp.clearToEOL();
  
  if(fontsize == 2)
    disp.set2X();
  else if(fontsize == 4)
  {
    disp.setFont(CalLite24); 
    disp.set2X();
  }
  else
    disp.set1X();
  
  disp.print(sym);
}

oledclass OLED = oledclass();
