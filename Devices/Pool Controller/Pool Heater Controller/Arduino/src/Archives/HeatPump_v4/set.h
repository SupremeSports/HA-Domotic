#ifndef set_h
  #define set_h
  
  class setclass
  {
    public:
      setclass();
      void serial();
      void ethernet();
      void oled();
      void setpins();
      void eeprom_read();
      void eeprom_write();
  };
  
  extern setclass set;

#endif
