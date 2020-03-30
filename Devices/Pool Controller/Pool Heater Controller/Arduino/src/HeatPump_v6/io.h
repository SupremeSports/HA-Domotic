#ifndef io_h
  #define io_h
  
  class ioclass
  {
    public:
      ioclass();
      void pump();
      void fan();
      void sequence();
      void read_sensors();
      void alarms(int temp_in, int temp_out);
  };
  
  extern ioclass io;

#endif
