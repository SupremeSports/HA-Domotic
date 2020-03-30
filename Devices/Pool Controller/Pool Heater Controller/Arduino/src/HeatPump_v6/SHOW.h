#ifndef show_h
  #define show_h
  
  class showclass
  {
    public:
      showclass();
      void set();
      void on();
      void off();
      void initial();
      void error();
      void hold();
      void lock();
      void mode();
      void temp_chg_base();
      void temp_chg();
      void temps_base();
      void temps_disp();
      void modes();
      void heating();
      void saved();
      void date();

      //ALARMS
      void alarm_hp();
      void alarm_lp();
      void alarm_fs();
      void alarm_dpc();
      void alarm_dpo();
      void alarm_flo();
      void alarm_off();
      void alarm_po();
      void alarm_pc();
  };
  
  extern showclass show;

#endif
