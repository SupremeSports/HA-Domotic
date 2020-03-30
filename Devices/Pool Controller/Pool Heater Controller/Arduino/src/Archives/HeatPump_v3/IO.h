#ifndef IO_h
  #define IO_h
  
  class ioclass
  {
    public:
      ioclass();
      void READSENSORS();
      void READBUTTONS();
      void BUTTONHOLD();
      void BUTTONDONE();
      void BUTTONUPDOWN();
      void BUTTONLOCK();
      void FAN();
      void SAVE_EEPROM();
      void READ_EEPROM();
      void PUMP();
      void SETPINS();
      void SHOW(int x);
  };
  
  extern ioclass io;

#endif
