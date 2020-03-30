#ifndef OLED_h
  #define OLED_h
  
  class oledclass
  {
    public:
      oledclass();
      void SET();
      void CLEAR();
      void CHAR(int x, int y, String sym, int fontsize, bool clearline);
      
  };
  
  extern oledclass OLED;

#endif
