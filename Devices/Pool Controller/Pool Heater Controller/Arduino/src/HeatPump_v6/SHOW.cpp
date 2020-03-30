#include "Arduino.h"

#include "OLED.h"
#include "SHOW.h"

#include "defines.h"
#include "variables.h"

showclass::showclass(){}

void showclass::set()
{
  #ifdef OLED_ENABLE
    OLED.SET();
    disp_on = true;
  #else
    disp_on = false;
  #endif
}

void showclass::on()
{
  #ifdef OLED_ENABLE
    OLED.DISPON();
    disp_on = true;
    show.date();
  #endif
}

void showclass::off()
{
  #ifdef OLED_ENABLE
    OLED.CLEAR();
    OLED.DISPOFF();
    disp_on = false;
  #endif
}

void showclass::initial()
{
  #ifdef OLED_ENABLE
    int scroll = 0;
    
    OLED.CHAR(16, 1, "STARTING", 2, true);
    OLED.CHAR(11, 3, "HEAT PUMP", 2, true);
    OLED.CHAR(28, 5, "SYSTEM", 2, true);
  
    do
    {
      OLED.CHAR(scroll, 7, ".", 1, (scroll == 0));
      scroll += 4;
      delay(25);          //Quick splashscreen
      //delay(50);          //Long splashscreen
    }while(scroll < 128);
  
    OLED.CLEAR();
    
    #ifdef ETHERNET_ENABLE
      datetime = "Date/Time wait...";
    #else
      datetime = "Heat Pump System";
    #endif
    
    OLED.CHAR(0, 0, datetime, 1, true);
  #endif
}

void showclass::error()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Buttons are locked", 1, true);
    OLED.CHAR(0, 2, "LOCK", 4, true);
  #endif
}

void showclass::hold()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Please hold button...", 1, true);
    OLED.CHAR(0, 2, "HOLD", 4, true);
  #endif
}

void showclass::lock()
{
  #ifdef OLED_ENABLE
    if(disp_locked)
    {
      OLED.CHAR(0, 1, "Buttons locked...", 1, true);
      OLED.CHAR(0, 2, "LOCK", 4, true);
    }
    else
    {
      OLED.CHAR(0, 1, "Buttons unlocked...", 1, true);
      OLED.CHAR(0, 2, "UNLK", 4, true);
    }
  #endif
}

void showclass::mode()
{
  #ifdef OLED_ENABLE
    if(spa_mode)
    {
      OLED.CHAR(0, 1, "SPA mode enabled", 1, true);
      OLED.CHAR(0, 2, "SPA", 4, true);
    }
    else
    {
      OLED.CHAR(0, 1, "POOL mode enabled", 1, true);
      OLED.CHAR(0, 2, "POOL", 4, true);
    }
  #endif
}

void showclass::temp_chg_base()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Requested Temperature", 1, true);
  #endif
}

void showclass::temp_chg()
{
  #ifdef OLED_ENABLE
    if(temp_req >= TEMP_MIN+1)
      OLED.CHAR(0, 2, String(temp_req,1)+" F", 4, true);
    else
      OLED.CHAR(0, 2, "OFF", 4, true);
  #endif
}

void showclass::temps_base()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "Tset:", 2, true);
    OLED.CHAR(115, 1, "F", 2, true);
    
    OLED.CHAR(0, 3, "Tin :", 2, true);
    OLED.CHAR(115, 3, "F", 2, true);
  
    OLED.CHAR(0, 5, "Tout:", 2, true);
    OLED.CHAR(115, 5, "F", 2, true);
  #endif
}

void showclass::temps_disp()
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

void showclass::modes()
{
  #ifdef OLED_ENABLE
    if(disp_locked)
      OLED.CHAR(120, 0, "L", 1, false);
    else
      OLED.CHAR(120, 0, " ", 1, false);
  
    if(spa_mode)
      OLED.CHAR(0, 7, "SPA", 1, true);
    else
      OLED.CHAR(0, 7, "POOL", 1, true);

    heating_on = false;
    heating_off = false;
    heating_stop = false;
  #endif
}

void showclass::heating()
{
  #ifdef OLED_ENABLE
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

void showclass::saved()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(30, 7, "Settings Saved!", 1, true);
    heating_on = false;
    heating_off = false;
    heating_stop = false;
  #endif
}

void showclass::date()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 0, datetime, 1, true);
    show.modes();
  #endif
}


//ALARMS
void showclass::alarm_hp()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: HIGH PRES ISSUE", 1, true);
    OLED.CHAR(0, 2, "HP", 4, true);
  #endif
}

void showclass::alarm_lp()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LOW PRES ISSUE", 1, true);
    OLED.CHAR(0, 2, "LP", 4, true);
  #endif
}

void showclass::alarm_fs()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LOW PRES FREEZE", 1, true);
    OLED.CHAR(0, 2, "FS", 4, true);
  #endif
}

void showclass::alarm_dpc()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LP TEMP PROBE SHORT", 1, true);
    OLED.CHAR(0, 2, "dPc", 4, true);
  #endif
}

void showclass::alarm_dpo()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: LP TEMP PROBE OPEN", 1, true);
    OLED.CHAR(0, 2, "dPo", 4, true);
  #endif
}

void showclass::alarm_flo()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER FLOW ISSUE", 1, true);
    OLED.CHAR(0, 2, "FLO", 4, true);
  #endif
}

/*void showclass::alarm_off()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "HEATING TURNED OFF", 1, true);
    OLED.CHAR(0, 2, "OFF", 4, true);
  #endif
}*/

void showclass::alarm_po()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER PROBE OPEN", 1, true);
    OLED.CHAR(0, 2, "Po", 4, true);
  #endif
}

void showclass::alarm_pc()
{
  #ifdef OLED_ENABLE
    OLED.CHAR(0, 1, "ERR: WATER PROBE SHORT", 1, true);
    OLED.CHAR(0, 2, "Pc", 4, true);
  #endif
}

showclass show = showclass();
