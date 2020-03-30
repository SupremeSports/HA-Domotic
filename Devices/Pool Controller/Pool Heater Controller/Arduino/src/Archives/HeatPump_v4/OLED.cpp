//PINOUT:
//SDA = A4
//SCL = A5
#include "Arduino.h"
#include "OLED.h"

#include "variables.h"

oledclass::oledclass(){}

void oledclass::DISPON()
{
  SSD1306_DISPLAYON;
  disp_on = true;
}

void oledclass::DISPOFF()
{
  SSD1306_DISPLAYOFF;
  disp_on = false;
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
