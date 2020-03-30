#include "Arduino.h"

#include "OLED.h"
#include "SHOW.h"

#include "defines.h"
#include "variables.h"

showclass::showclass(){}

bool showclass::set()
{
  #ifdef OLED_ENABLE
    return(OLED.SET());
  #else
    return(false);
  #endif
}

void showclass::blank()
{
  #ifdef OLED_ENABLE
    OLED.CLEAR();
  #endif
}

bool showclass::on()
{
  #ifdef OLED_ENABLE
    return(OLED.DISPON());
  #endif
}

bool showclass::off()
{
  #ifdef OLED_ENABLE
    return(OLED.DISPOFF());
  #endif
}

void showclass::initial()
{
  #ifdef OLED_ENABLE
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
      OLED.CHAR(0, 0, "Date/Time wait...", 1, true);
    #else
      OLED.CHAR(0, 0, "Heat Pump System", 1, true);
    #endif
  #endif
}

void showclass::error()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Buttons are locked", 1, true);
    OLED.CHAR(0, 2, "LOCK ", 4, true);
  #endif
}

void showclass::hold()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Please hold button...", 1, true);
    OLED.CHAR(0, 2, "HOLD ", 4, true);
  #endif
}

void showclass::lock(bool disp_locked)
{
  #ifdef OLED_ENABLE
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
  #endif
}

void showclass::mode(bool spa_mode)
{
  #ifdef OLED_ENABLE
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
  #endif
}

void showclass::temp_req_base()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Requested Temperature", 1, true);
  #endif
}

void showclass::temp_req(float temp_req)
{
  #ifdef OLED_ENABLE
    if(temp_req >= TEMP_MIN+1)
      OLED.CHAR(0, 2, String(temp_req,1)+" F ", 4, false);
    else
      OLED.CHAR(0, 2, "OFF   ", 4, false);
  #endif
}

void showclass::temps_base()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Tset:", 2, true);
    OLED.CHAR(115, 1, "F ", 2, true);
    
    OLED.CHAR(0, 3, "Tin :", 2, true);
    OLED.CHAR(115, 3, "F ", 2, true);
  
    OLED.CHAR(0, 5, "Tout:", 2, true);
    OLED.CHAR(115, 5, "F ", 2, true);
  #endif
}

void showclass::temps(float temp_req, float temp_in_F, float temp_out_F, bool heat_enable)
{
  #ifdef OLED_ENABLE
    if(temp_req >= TEMP_MIN+1 && heat_enable)
      OLED.CHAR(60, 1, String(temp_req,1), 2, false);
    else
      OLED.CHAR(60, 1, "----", 2, false);
  
    OLED.CHAR(60, 3, String(temp_in_F,1), 2, false);
  
    OLED.CHAR(60, 5, String(temp_out_F,1), 2, false);
  #endif
}

void showclass::modes(bool disp_locked, bool spa_mode)
{
  #ifdef OLED_ENABLE
    if(disp_locked)
      OLED.CHAR(120, 0, "L", 1, false);
    else
      OLED.CHAR(120, 0, " ", 1, false);
  
    if(spa_mode)
      OLED.CHAR(0, 7, "SPA ", 1, true);
    else
      OLED.CHAR(0, 7, "POOL", 1, true);
  #endif
}

void showclass::heating(int seq_count, bool heat_enable, bool pump_out, bool alarm_on)
{
  #ifdef OLED_ENABLE
    //int dot_position = (0.0133*seq_count) + 70; // 10 points
    int dot_position = (0.008*seq_count) + 70; // 3 points

    if(alarm_on)
    {
      heating_on = false;
      heating_off = false;
      heating_stop = false;
      return;
    }

    if(heat_enable)
    {
      if(pump_out)
      {
        if(!heating_on)
        {
          heating_on = true;
          heating_off = false;
          heating_stop = false;
          OLED.CHAR(30, 7, "Heating", 1, true);
        }

        if(seq_count == DISP_RUN)
          OLED.CHAR(dot_position, 7, "", 1, true);
        //if(seq_count > DISP_RUN)                                                                //Scroling point
        if(dot_position == 74 || dot_position == 78 || dot_position == 82 || dot_position == 86)  //Adding 3 points
          OLED.CHAR(dot_position, 7, ".", 1, false);
      }
      else if(!heating_stop)
      {
        heating_on = false;
        heating_off = false;
        heating_stop = true;
        OLED.CHAR(30, 7, "Heat Stopped", 1, true);
      }
    }
    else if(!heating_off)
    {
      heating_on = false;
      heating_off = true;
      heating_stop = false;
      OLED.CHAR(30, 7, "Heating Off", 1, true);
    }
  #endif
}

void showclass::saved(bool save_eeprom)
{
  OLED.CHAR(30, 7, "Settings Saved!", 1, true);
  heating_on = false;
  heating_off = false;
  heating_stop = false;
}

void showclass::date(String datetime)
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 0, datetime, 1, false);
  #endif
}


//ALARMS
void showclass::alarm_hp()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: HIGH PRES ISSUE", 1, true);
    OLED.CHAR(0, 2, "HP   ", 4, true);
  #endif
}

void showclass::alarm_lp()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LOW PRES ISSUE", 1, true);
    OLED.CHAR(0, 2, "LP   ", 4, true);
  #endif
}

void showclass::alarm_fs()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LOW PRES FREEZE", 1, true);
    OLED.CHAR(0, 2, "FS   ", 4, true);
  #endif
}

void showclass::alarm_dpc()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LP TEMP PROBE SHORT", 1, true);
    OLED.CHAR(0, 2, "dPc  ", 4, true);
  #endif
}

void showclass::alarm_dpo()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LP TEMP PROBE OPEN", 1, true);
    OLED.CHAR(0, 2, "dPo  ", 4, true);
  #endif
}

void showclass::alarm_flo()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER FLOW ISSUE", 1, true);
    OLED.CHAR(0, 2, "FLO  ", 4, true);
  #endif
}

void showclass::alarm_off()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "HEATING TURNED OFF", 1, true);
    OLED.CHAR(0, 2, "OFF  ", 4, true);
  #endif
}

void showclass::alarm_po()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER PROBE OPEN", 1, true);
    OLED.CHAR(0, 2, "Po  ", 4, true);
  #endif
}

void showclass::alarm_pc()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER PROBE SHORT", 1, true);
    OLED.CHAR(0, 2, "Pc  ", 4, true);
  #endif
}

showclass show = showclass();
