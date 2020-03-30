#ifndef show_h
  #define show_h
  
  class showclass
  {
    public:
      showclass();
      bool set();
      void blank();
      bool on();
      bool off();
      void initial();
      void error();
      void hold();
      void lock(bool disp_locked);
      void mode(bool spa_mode);
      void lock();
      void mode();
      void temp_req_base();
      void temp_req(float temp_req);
      void temps_base();
      void temps(float temp_req, float temp_in_F, float temp_out_F, bool heat_enable);
      void modes(bool disp_locked, bool spa_mode);
      void heating(int seq_count, bool heat_enable, bool pump_out, bool alarm_on);
      void saved(bool save_eeprom);
      void date(String datetime);

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
