#include "arduino.h"
#include "IO.h"

#include "defines.h"
#include "variables.h"

#include "OLED.h"

ioclass::ioclass(){}

void ioclass::READSENSORS()
{
  temp_in = (int)analogRead(TEMP_IN_WATER);
  temp_in_F = (ratio_ax2 * temp_in * temp_in) + (ratio_bx * temp_in) + ratio_c;
  
  temp_out = (int)analogRead(TEMP_OUT_WATER);
  temp_out_F = (ratio_ax2 * temp_out * temp_out) + (ratio_bx * temp_out) + ratio_c;

  //Safety devices
  flow = digitalRead(SWITCH_FLOW);
  high_pres = digitalRead(SWITCH_HP);
  low_pres = digitalRead(SWITCH_LP);
  temp_lp = (int)analogRead(TEMP_LP);
}

void ioclass::READBUTTONS()
{
  //Panel mounted buttons
  button = (int)analogRead(BUTTONS);

  button_lock = (button >= Lock_min && button <= Lock_max);
  button_up = (button >= Up_min && button <= Up_max);
  button_down = (button >= Down_min && button <= Down_max);
  button_spa = (button >= Spa_min && button <= Spa_max);
}

void ioclass::BUTTONHOLD()
{
  if(!hold_button)
    count_hold = 0;
  else
    count_hold++;
  
  hold_button = true;
  Serial.print("HOLD: ");
  Serial.println(count_hold);
  delay(100);
  if(count_hold < 20)
    return;
    
  hold_done = true;

  if(button_lock)
    lock_request = true;

  if(button_spa)
    spa_request = true;
}

void ioclass::BUTTONDONE()
{
  if(!button_lock && !button_spa)
  {
    hold_release = false;
    lock_request = false;
    spa_request = false;
    hold_done = false;
  }

  if(hold_release)
    return;

  if(lock_request)
  {
    if(!disp_locked)
    {
      disp_locked = true; 
      #ifdef OLED_ENABLE
        OLED.CHAR(0, 1, "Buttons locked", 1, true);
        OLED.CHAR(0, 2, "LOCK  ", 4, true);
      #endif
    }
    else
    {
      disp_locked = false; 
      #ifdef OLED_ENABLE
        OLED.CHAR(0, 1, "Buttons unlocked", 1, true);
        OLED.CHAR(0, 2, "UNLK  ", 4, true);
      #endif
    }
    hold_release = true;
    count_display = 0;
    save_eeprom = true;;
  }
  else if(spa_request)
  {
    if(!spa_mode)
    {
      spa_mode = true; 
      #ifdef OLED_ENABLE
        OLED.CHAR(0, 1, "SPA mode enabled", 1, true);
        OLED.CHAR(0, 2, "SPA  ", 4, true);
      #endif
    }
    else
    {
      spa_mode = false; 
      #ifdef OLED_ENABLE
        OLED.CHAR(0, 1, "POOL mode enabled", 1, true);
        OLED.CHAR(0, 2, "POOL ", 4, true);
      #endif
    }
    hold_release = true;
    count_display = 0;
    save_eeprom = true;
  }
}

void ioclass::BUTTONUPDOWN()
{
  
}

void ioclass::BUTTONFAIL()
{
  Serial.println("BUTTONS ARE LOCKED");
}

void ioclass::SHOW(int x)
{
  if(!hold_done && count_hold == 0 && x == 1)
  {
    OLED.CHAR(0, 1, "Please hold button...", 1, true);
    OLED.CHAR(0, 2, "HOLD ", 4, true);
  }

  if(x == 2)
  {
    OLED.CHAR(0, 1, "Tset:", 2, true);
    OLED.CHAR(115, 1, "F ", 2, true);
    
    OLED.CHAR(0, 3, "Tin :", 2, true);
    OLED.CHAR(115, 3, "F ", 2, true);
  
    OLED.CHAR(0, 5, "Tout:", 2, true);
    OLED.CHAR(115, 5, "F ", 2, true);
  }
  if(x == 3)
  {
    String Treq = String(temp_req,1);
    String Tin = String(temp_in_F,1);
    String Tout = String(temp_out_F,1);
  
    OLED.CHAR(60, 1, Treq, 2, false);
  
    OLED.CHAR(60, 3, Tin, 2, false);
  
    OLED.CHAR(60, 5, Tout, 2, false);
  }

  if(x == 4)
  {
    if(disp_locked)
      OLED.CHAR(120, 0, "L", 1, false);
    else
      OLED.CHAR(120, 0, " ", 1, false);

    if(spa_mode)
      OLED.CHAR(0, 7, "SPA ", 1, true);
    else
      OLED.CHAR(0, 7, "POOL", 1, true);
  }

  if(x == 5)
  {
    if(false)//(pump_out && heat_enable)
    {
      if(count_display == 6)
      {
        OLED.CHAR(30, 7, "Heating", 1, true);
        OLED.CHAR(74, 7, "", 1, true);
      }
      if(count_display == 7)
        OLED.CHAR(74, 7, ".", 1, false);
      if(count_display == 8)
        OLED.CHAR(78, 7, ".", 1, false);
      if(count_display == 9)
        OLED.CHAR(82, 7, ".", 1, false);
    }
    else
    {
      if(count_display == 6)
        OLED.CHAR(30, 7, "Heat Stopped", 1, true);
    }
      
  }
  
  if(x == 6)
  {
    OLED.CHAR(0, 7, "SETTINGS SAVED!", 1, true);
  }
}

void ioclass::FAN()
{
  
}

void ioclass::PUMP()
{
  
}

ioclass io = ioclass();
