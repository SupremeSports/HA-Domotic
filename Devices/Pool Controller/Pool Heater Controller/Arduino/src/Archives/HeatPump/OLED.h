#ifndef OLED_h
  #define OLED_h
  
  class oledclass
  {
    public:
      oledclass();
      /*
      void oledclass::DISP(int NbrLines, String Line1, String Line2, String Line3, String Line4, String Line5);
      void oledclass::CONT(int level);
      */
      void oledclass::DISPON();
      void oledclass::DISPOFF();
      void oledclass::CHAR(int x, int y, String sym, int fontsize, bool clearline);
  };
  
  extern oledclass OLED;

#endif
