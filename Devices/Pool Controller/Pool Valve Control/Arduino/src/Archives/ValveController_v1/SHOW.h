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
      void saved();
      void date();

      void base(int disp);
      void data_disp();
      void manauto();

      void temp_chg_base();
      void temp_chg();
      void error();
      void hold();
      void lock();

      //ALARMS
      
  };
  
  extern showclass show;

#endif
