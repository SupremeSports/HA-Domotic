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

      void clear_lines(int select, int line_start, int line_end);
      void base(int disp);
      void data_disp();
      void manauto();

      void temp_chg_base();
      void temp_chg();
      void error();
      void hold();
      void lock();

      void locked_status();
      
      void single_row(int disp_select);
      void dual_row(int disp_select);

      //ALARMS
      
  };
  
  extern showclass show;

#endif
