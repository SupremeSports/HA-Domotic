#include "Arduino.h"

#include "OLED.h"
#include "SHOW.h"
#include "io.h"

#include "defines.h"
#include "variables.h"

int toggle = 0;

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
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);

      show.date();
    }
    disp_on = true;
  #endif
}

void showclass::off()
{
  #ifdef OLED_ENABLE
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);
      
      OLED.CLEAR();
    }
    disp_on = false;
  #endif
}

void showclass::clear_lines(int select, int line_start, int line_end)
{
  for(int line = line_start; line<=line_end; line++)
  {
    OLED.CHAR(0, line, "", 1, true);
    if(line == 7)
    {
      manual[select] = false;
      automatic[select] = false;
      disabled[select] = false;
    }
  }
}

void showclass::base(int disp)
{
  #ifdef OLED_ENABLE
    io.I2Cselect(disp);                         //Select appropriate I2C channel

    show.clear_lines(disp, 1, 7);

    if(disp == Disp_Solar)
      OLED.CHAR(6, 1, "SOLAR VALVE CONTROL", 1, true);
    else if(disp == Disp_Heater)
      OLED.CHAR(2, 1, "HEATPUMP VALVE CONTROL", 1, true);
    else if(disp == Disp_Mixer)
      OLED.CHAR(6, 1, "MIXER VALVE CONTROL", 1, true);

    toggle = 0;
  #endif
}

void showclass::data_disp()
{  
  #ifdef OLED_ENABLE
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);

      #ifdef DUAL_ROW_DISP
        dual_row(disp_select);
      #else
        single_row(disp_select);
      #endif
    }
    toggle++;
    if(toggle > 2)
      toggle = 0;
  #endif
}

void showclass::manauto()
{
  #ifdef OLED_ENABLE    
    int dot_base;
    int dot_position;

    bool specific_enable;

    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      if(disp_select == Disp_Solar)
        specific_enable = solar_enable;
      else if(disp_select == Disp_Heater)
        specific_enable = heatpump_enable; 
      else if(disp_select == Disp_Mixer)
        specific_enable = true;
      else
        break;

      io.I2Cselect(disp_select);    //Select appropriate I2C channel

      if(specific_enable)
      {
        if(Sw_Mtr[disp_select])
        {
          if(!manual[disp_select])
          {
            manual[disp_select] = true;
            automatic[disp_select] = false;
            disabled[disp_select] = false;
            OLED.CHAR(0, 7, "Manual", 1, true);
          }
        }
        else if(!automatic[disp_select])
        {
          manual[disp_select] = false;
          automatic[disp_select] = true;
          disabled[disp_select] = false;
          OLED.CHAR(0, 7, "Automatic", 1, true);
        }
        
        dot_base = automatic[disp_select] ? 55 : 35;
        dot_position = (0.032*seq_count) + dot_base; // 3 points

        if(seq_count == DISP_RUN)
          OLED.CHAR(dot_position, 7, "", 1, true);
        //if(seq_count > DISP_RUN)                                                                //Scroling point
        if(dot_position == dot_base+4 || dot_position == dot_base+8 || dot_position == dot_base+12 || dot_position == dot_base+16)  //Adding 3 points
          OLED.CHAR(dot_position, 7, ".", 1, false);
      }
      else// if(!disabled[disp_select])
      {
        manual[disp_select] = false;
        automatic[disp_select] = false;
        disabled[disp_select] = true;
        OLED.CHAR(0, 7, "Disabled", 1, true);
      }
    }
  #endif
}

void showclass::initial()
{
  #ifdef OLED_ENABLE
    int scroll = 0;

    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);    //Select appropriate I2C channel

      OLED.CHAR(16, 1, "STARTING", 2, true);
      OLED.CHAR(28, 3, "VALVES", 2, true);
      OLED.CHAR(28, 5, "SYSTEM", 2, true);
    }

    do
    {
      for(int disp_select = 0; disp_select<3; disp_select++)
      {
        io.I2Cselect(disp_select);
        OLED.CHAR(scroll, 7, ".", 1, (scroll == 0));
      }
      scroll += 4;
      true ? delay(25) : delay(50);
    }while(scroll < 128);

    #ifdef ETHERNET_ENABLE
      datetime = "Date/Time wait...";
    #else
      datetime = "Valve Control System";
    #endif
  
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);
      OLED.CLEAR();
      OLED.CHAR(0, 0, datetime, 1, true);
      show.base(disp_select);
      show.locked_status();
    }
  #endif
}

void showclass::temp_chg_base()
{
  #ifdef OLED_ENABLE
    io.I2Cselect(Disp_Mixer);

    show.clear_lines(Disp_Mixer, 1, 7);
    
    OLED.CHAR(0, 1, "Requested Temperature", 1, true);
  #endif
}

void showclass::temp_chg()
{
  #ifdef OLED_ENABLE
    io.I2Cselect(Disp_Mixer);
    if(temp_req >= TEMP_MIN+1)
      OLED.CHAR(0, 3, String(temp_req,1)+" F", 4, true);
    else
      OLED.CHAR(0, 3, "OFF", 4, true);
  #endif
}

void showclass::error()
{
  #ifdef OLED_ENABLE
    io.I2Cselect(Disp_Mixer);
    
    show.clear_lines(Disp_Mixer, 1, 7);
    
    OLED.CHAR(0, 1, "Buttons are locked", 1, true);
    OLED.CHAR(0, 3, "LOCK", 4, true);
  #endif
}

void showclass::hold()
{
  #ifdef OLED_ENABLE
    io.I2Cselect(Disp_Mixer);

    show.clear_lines(Disp_Mixer, 1, 7);
    
    OLED.CHAR(0, 1, "Please hold button...", 1, true);
    OLED.CHAR(0, 3, "HOLD", 4, true);
  #endif
}

void showclass::lock()
{
  #ifdef OLED_ENABLE
    io.I2Cselect(Disp_Mixer);

    show.clear_lines(Disp_Mixer, 1, 7);
    
    if(disp_locked)
    {
      OLED.CHAR(0, 1, "Buttons locked...", 1, true);
      OLED.CHAR(0, 3, "LOCK", 4, true);
    }
    else
    {
      OLED.CHAR(0, 1, "Buttons unlocked...", 1, true);
      OLED.CHAR(0, 3, "UNLK", 4, true);
    }

    show.locked_status();
  #endif
}

void showclass::saved()
{
  #ifdef OLED_ENABLE
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      OLED.CHAR(0, 7, "Settings Saved!", 1, true);
      manual[disp_select] = false;
      automatic[disp_select] = false;
      disabled[disp_select] = false;
    }
  #endif
}

void showclass::locked_status()
{
  #ifdef OLED_ENABLE
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);

      if(disp_locked)
        OLED.CHAR(120, 0, "L", 1, false);
      else
        OLED.CHAR(120, 0, " ", 1, false);
    }
  #endif
}

void showclass::date()
{
  #ifdef OLED_ENABLE
    for(int disp_select = 0; disp_select<3; disp_select++)
    {
      io.I2Cselect(disp_select);

      OLED.CHAR(0, 0, datetime, 1, true);
    }
    show.locked_status();
  #endif
}

void showclass::single_row(int disp_select)
{  
  show.clear_lines(disp_select, 2, 2);
  show.clear_lines(disp_select, 4, 6);
  
  if(toggle == 0)
  {
    if(disp_select == Disp_Solar)
    {
      OLED.CHAR(10, 2, "Solar", 1, false);
      OLED.CHAR(75, 2, "Bypass", 1, false);
    }

    if(disp_select == Disp_Heater)
    {
      OLED.CHAR(8, 2, "Heater", 1, false);
      OLED.CHAR(75, 2, "Bypass", 1, false);
    }

    if(disp_select == Disp_Mixer)
    {
      OLED.CHAR(10, 2, "Drain", 1, false);
      OLED.CHAR(70, 2, "Skimmer", 1, false);
    }

    OLED.CHAR(10, 4, String(Percent_Mtr[disp_select])+"%", 2, false);
    OLED.CHAR(75, 4, String(100-Percent_Mtr[disp_select])+"%", 2, false);
  }
  else if(toggle == 1)
  {
    OLED.CHAR(10, 2, "Before", 1, false);
    OLED.CHAR(80, 2, "After", 1, false);
    
    if(disp_select == Disp_Solar)
    {
      OLED.CHAR(5, 4, String(Temp_Inlet, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(70, 4, String(Temp_Solar, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }

    if(disp_select == Disp_Heater)
    {
      OLED.CHAR(5, 4, String(Temp_Middle, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(70, 4, String(Temp_Heater, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }

    if(disp_select == Disp_Mixer)
    {
      OLED.CHAR(5, 4, String(Temp_Inlet, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(70, 4, String(Temp_Return, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }
  }
  else if(toggle == 2)
  {
    OLED.CHAR(10, 2, "Temperature preset", 1, false);
    OLED.CHAR(30, 4, String(temp_req,1)+" F", 2, false);
  }
}

void showclass::dual_row(int disp_select)
{
  show.clear_lines(disp_select, 2, 6);
  if(toggle == 0)
  {
    if(disp_select == Disp_Solar)
    {
      OLED.CHAR(0, 3, "Solar :", 1, false);
      OLED.CHAR(0, 5, "Bypass:", 1, false);
    }
    if(disp_select == Disp_Heater)
    {
      OLED.CHAR(0, 3, "Heater:", 1, false);
      OLED.CHAR(0, 5, "Bypass:", 1, false);
    }
    if(disp_select == Disp_Mixer)
    {
      OLED.CHAR(0, 3, "Drain :", 1, false);
      OLED.CHAR(0, 5, "Skim  :", 1, false);
    }
    OLED.CHAR(60, 2, String(Percent_Mtr[disp_select])+"%", 2, false);
    OLED.CHAR(60, 4, String(100-Percent_Mtr[disp_select])+"%", 2, false);
  }
  else if(toggle == 1)
  {
    OLED.CHAR(0, 3, "Before:", 1, false);
    OLED.CHAR(0, 5, "After :", 1, false);
    
    if(disp_select == Disp_Solar)
    {
      OLED.CHAR(60, 2, String(Temp_Inlet, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(60, 4, String(Temp_Solar, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }
    if(disp_select == Disp_Heater)
    {
      OLED.CHAR(60, 2, String(Temp_Middle, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(60, 4, String(Temp_Heater, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }
    if(disp_select == Disp_Mixer)
    {
      OLED.CHAR(60, 2, String(Temp_Inlet, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
      OLED.CHAR(60, 4, String(Temp_Return, (Temp_Return < 100 ? 1 : 0))+"F", 2, false);
    }
  }
  else if(toggle == 2)
  {
    OLED.CHAR(0, 4, "Preset:", 1, false);
    OLED.CHAR(60, 3, String(temp_req,1)+" F", 2, false);
  }
}


//ALARMS


showclass show = showclass();
