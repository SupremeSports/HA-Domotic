#ifndef io_h
  #define io_h
  
  class ioclass
  {
    public:
      ioclass();
      void pump();
      void fan();
      void alarms(int temp_in, int temp_out);
      void sequence();
      void read_sensors();
  };
  
  extern ioclass io;

#endif
