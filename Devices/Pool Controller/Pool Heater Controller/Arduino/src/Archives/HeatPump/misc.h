void savetoEEPROM()
{
  spalock_modes = 0;
  if(spa_mode)
    spalock_modes += 1;
  if(disp_locked)
    spalock_modes += 10;
  if(heat_enable)
    spalock_modes += 100;

  //Convert temperature to lower number since the maximum eeprom value is 255 and temp has to be multiplied by 10 to accept decimal    
  int save_temp_req = (temp_req-temp_min)*10;
  
  EEPROM.write(0, byte(save_temp_req));
  EEPROM.write(1, byte(spalock_modes));
  
  eeprom_req_saved = true;
  disp_saved = false;
  count = 0;
}

void disptemps()
{
  String Treq = String(temp_req,1);
  String Tin = String(temp_in_F,1);
  String Tout = String(temp_out_F,1);

  OLED.CHAR(60, 1, Treq, 2, false);

  OLED.CHAR(60, 3, Tin, 2, false);

  OLED.CHAR(60, 5, Tout, 2, false);
}

void dispbase()
{
  OLED.CHAR(0, 1, "Tset:", 2, true);
  OLED.CHAR(115, 1, "F ", 2, true);
  
  OLED.CHAR(0, 3, "Tin :", 2, true);
  OLED.CHAR(115, 3, "F ", 2, true);

  OLED.CHAR(0, 5, "Tout:", 2, true);
  OLED.CHAR(115, 5, "F ", 2, true);
}

void displock()
{
  if(disp_locked)
    OLED.CHAR(120, 0, "L", 1, false);
  else
    OLED.CHAR(120, 0, " ", 1, false);
}

void dispmode()
{
  if(spa_mode)
    OLED.CHAR(0, 7, "SPA ", 1, false);
  else
    OLED.CHAR(0, 7, "POOL", 1, false);
}

void heatstatus(bool saved)
{
  if(saved)
  {
    if(pump_out && heat_enable)
    {
      if(count == 0)
      {
        OLED.CHAR(30, 7, "Heating", 1, true);
        OLED.CHAR(74, 7, "", 1, true);
      }
      if(count == 10)
        OLED.CHAR(74, 7, ".", 1, false);
      if(count == 20)
        OLED.CHAR(78, 7, ".", 1, false);
      if(count == 30)
        OLED.CHAR(82, 7, ".", 1, false);
      }
      else
        OLED.CHAR(25, 7, "Heat Stopped", 1, true);
  }
  else
  {
    if(count == 0)
    {
      dispbase();
      disptemps();
      OLED.CHAR(0, 7, "SETTINGS SAVED!", 1, true);
    }
    else if(count == 40)
      saved = true;
  }

  #ifdef OLED_ENABLE
    if(count == 1)
      disptemps();
    
    if(count == 42)
      displock();

    if(count >= 80)
      dispmode();
    
    if(count == 85)
      dispbase();
  #endif

  count++;

  if(count > 40)
    count = 0;
}

void readsensors()
{
  temp_in = (int)analogRead(TEMP_IN_WATER);
  temp_in_F = (ratio_ax2 * temp_in * temp_in) + (ratio_bx * temp_in) + ratio_c;
  
  temp_out = (int)analogRead(TEMP_OUT_WATER);
  temp_out_F = (ratio_ax2 * temp_out * temp_out) + (ratio_bx * temp_out) + ratio_c;

  //User buttons
  button = (int)analogRead(BUTTONS);

  //Safety devices
  flow = digitalRead(SWITCH_FLOW);
  high_pres = digitalRead(SWITCH_HP);
  low_pres = digitalRead(SWITCH_LP);
  temp_lp = (int)analogRead(TEMP_LP);
}
