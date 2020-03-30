// ----------------------------------------------------------------------------------------------------
// --------------------------------------- NEOPIXEL PATTERNS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
// Light a dot with a color
void colorWipe(uint32_t c, uint16_t i)
{
    if (i >= strip.numPixels())
      return;
    strip.setPixelColor(i, c);
    strip.show();
}

// Fill all dots with a color
void colorFill(uint32_t c)
{
    for (int i=0; i<strip.numPixels(); i++)
      strip.setPixelColor(i, c);

    strip.show();
}

void rainbow(uint16_t j)
{
  for(uint16_t i=0; i<strip.numPixels(); i++)
    strip.setPixelColor(i, Wheel((i+j) & 255));
    
  strip.show();
}

void rainbowAll(uint8_t wait)
{
  uint16_t i, j;

  for(j=0; j<256; j++)
  {
    for(i=0; i<strip.numPixels(); i++)
      strip.setPixelColor(i, Wheel((i+j) & 255));
      
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycleAll(uint8_t wait)
{
  uint16_t i, j;

  for(j=0; j<256*5; j++)
  { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++)
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));

    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85)
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);

  if(WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
