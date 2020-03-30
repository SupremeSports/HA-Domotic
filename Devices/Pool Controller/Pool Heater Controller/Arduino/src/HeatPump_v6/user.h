#ifndef user_h
  #define user_h
  
  class userclass
  {
    public:
      userclass();
      void read_buttons();
      void button_work();
      void error();
      void hold();
      void updown();
      void lock();
      void mode();
  };
  
  extern userclass user;

#endif
