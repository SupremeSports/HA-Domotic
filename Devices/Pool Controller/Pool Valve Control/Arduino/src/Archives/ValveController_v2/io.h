#ifndef io_h
  #define io_h
  
  class ioclass
  {
    public:
      ioclass();

      void sequence();
      
      void read_sensors();
      
      int calc_solar();
      int calc_heatpump();
      int calc_inlet();
      
      void move_motors();
      
      void I2Cselect(uint8_t i);
      
      int pulseWidth(int angle);

      void set_mux();
      void set_pwm();
  };
  
  extern ioclass io;

#endif
