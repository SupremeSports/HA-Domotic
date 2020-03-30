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
      void BUTTONFAIL();
      void SHOW(int x);
      void FAN();
      void PUMP();
  };
  
  extern ioclass io;

#endif
