#include "Arduino.h"

#include "OLED.h"
#include "SHOW.h"

#include "defines.h"

bool heating_disp;

showclass::showclass(){}

void showclass::blank()
{
  OLED.CLEAR();
}

void showclass::initial()
{
  int startup = 0;
  
  OLED.CHAR(16, 1, "STARTING", 2, true);
  OLED.CHAR(11, 3, "HEAT PUMP", 2, true);
  OLED.CHAR(28, 5, "SYSTEM", 2, true);

  do
  {
    OLED.CHAR(startup, 7, ".", 1, (startup == 0));
    startup += 4;
    delay(50);
  }while(startup < 128);

  OLED.CLEAR();
  
  #ifdef ETHERNET_ENABLE
    OLED.CHAR(0, 0, "Date/Time wait...", 1, false);
  #else
    OLED.CHAR(0, 0, "Heat Pump System", 1, false);
  #endif
}

void showclass::error()
{
  OLED.CHAR(0, 1, "Buttons are locked", 1, true);
  OLED.CHAR(0, 2, "LOCK ", 4, true);
}

void showclass::hold()
{
  OLED.CHAR(0, 1, "Please hold button...", 1, true);
  OLED.CHAR(0, 2, "HOLD ", 4, true);
}

void showclass::lock(bool disp_locked)
{
  if(disp_locked)
  {
    OLED.CHAR(0, 1, "Buttons locked...", 1, true);
    OLED.CHAR(0, 2, "LOCK ", 4, true);
  }
  else
  {
    OLED.CHAR(0, 1, "Buttons unlocked...", 1, true);
    OLED.CHAR(0, 2, "UNLK ", 4, true);
  }
}

void showclass::spa(bool spa_mode)
{
  if(spa_mode)
  {
    OLED.CHAR(0, 1, "SPA mode enabled", 1, true);
    OLED.CHAR(0, 2, "SPA  ", 4, true);
  }
  else
  {
    OLED.CHAR(0, 1, "POOL mode enabled", 1, true);
    OLED.CHAR(0, 2, "POOL ", 4, true);
  }
}

void showclass::temp_req_base()
{
  OLED.CHAR(0, 1, "Requested Temperature", 1, true);
}

void showclass::temp_req(float temp_req)
{
  OLED.CHAR(0, 2, String(temp_req,1)+" F ", 4, false);
}

void showclass::temps_base()
{
  OLED.CHAR(0, 1, "Tset:", 2, true);
  OLED.CHAR(115, 1, "F ", 2, true);
  
  OLED.CHAR(0, 3, "Tin :", 2, true);
  OLED.CHAR(115, 3, "F ", 2, true);

  OLED.CHAR(0, 5, "Tout:", 2, true);
  OLED.CHAR(115, 5, "F ", 2, true);
}

void showclass::temps(float temp_req, float temp_in_F, float temp_out_F)
{
  String Treq = String(temp_req,1);
  String Tin = String(temp_in_F,1);
  String Tout = String(temp_out_F,1);

  OLED.CHAR(60, 1, Treq, 2, false);

  OLED.CHAR(60, 3, Tin, 2, false);

  OLED.CHAR(60, 5, Tout, 2, false);
}

void showclass::modes(bool disp_locked, bool spa_mode)
{
  if(disp_locked)
    OLED.CHAR(120, 0, "L", 1, false);
  else
    OLED.CHAR(120, 0, " ", 1, false);

  if(spa_mode)
    OLED.CHAR(0, 7, "SPA ", 1, true);
  else
    OLED.CHAR(0, 7, "POOL", 1, true);

  heating_disp = false;
}

void showclass::heating(int seq_count)
{
  int dot_position = (4*seq_count) - 314;
  
  if(true)//(pump_out && heat_enable)
  {
    if(seq_count == DISP_RUN && !heating_disp)
    {
      OLED.CHAR(30, 7, "Heating", 1, true);
      heating_disp = true;
    }
    if(seq_count == DISP_RUN && heating_disp)
      OLED.CHAR(dot_position, 7, "", 1, true);
    if(seq_count > DISP_RUN)
      OLED.CHAR(dot_position, 7, ".", 1, false);
  }
  else
  {
    if(seq_count == DISP_RUN && heating_disp)
      OLED.CHAR(30, 7, "Heat Stopped", 1, true);

    heating_disp = false;
  }
}

void showclass::date(String datetime)
{
  
}

showclass show = showclass();
