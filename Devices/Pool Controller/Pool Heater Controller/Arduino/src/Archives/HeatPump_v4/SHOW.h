#ifndef show_h
  #define show_h
  
  class showclass
  {
    public:
      showclass();
      void blank();
      void initial();
      void error();
      void hold();
      void lock(bool disp_locked);
      void spa(bool spa_mode);
      void temp_req_base();
      void temp_req(float temp_req);
      void temps_base();
      void temps(float temp_req, float temp_in_F, float temp_out_F);
      void modes(bool disp_locked, bool spa_mode);
      void heating(int seq_count);
      void date(String datetime);

  };
  
  extern showclass show;

#endif
