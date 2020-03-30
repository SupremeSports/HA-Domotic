#include "arduino.h"
#include "IO.h"

#include "defines.h"
#include "variables.h"

#include "OLED.h"

#include <EEPROM.h>

ioclass::ioclass(){}

void ioclass::READBUTTONS()
{
  if(!updn_request)
    delay(LOOP_TIME);
  else
    delay(LOOP_TIME/LOOP_HOLD);
  
  //READ panel mounted buttons
  button = (int)analogRead(BUTTONS);

  //Convert analog buttons to actual boolean buttons
  button_lock = (button >= Lock_min && button <= Lock_max);
  button_up = (button >= Up_min && button <= Up_max);
  button_down = (button >= Down_min && button <= Down_max);
  button_spa = (button >= Spa_min && button <= Spa_max);

  if(button < 20 || updn_request)
  {    
    hold_done = false;

    if((hold_request || disp_error) && !hold_release)
      seq_count = DISP_BASE_NOW;
      
    hold_request = false;
    disp_error = false;

    hold_release = false;

    done_error = false;

    hold_count = 0;
  }

  if(button > 20)
    save_count = 0;

  //Detecting button press while locked
  if((button_spa || button_up || button_down) && disp_locked)
  {
    if(!disp_error)
    {
      io.SHOW(8);
      hold_count = 0;
      disp_error = true;
    }
    return;
  }

  if((button_lock || button_spa) && !hold_request)
  {
      io.SHOW(1);
      hold_count = 0;
      hold_request = true;

      updn_request = false;
  }

  if((button_up || button_down) && !updn_request)
  {
    updn_count = 0;
    updn_request = true;

    io.SHOW(12);

    Serial.println("UP/DOWN REQUEST");
  }
  
}

void ioclass::BUTTONUPDOWN()
{
  if(!updn_request)
    return;
/*
  if(updn_request && save_eeprom)
  {
    if(button_up || button_down)
      return;
    else
    {
      updn_request = false;
      return;
    }
  }
*/

  Serial.println("UP/DOWN INSIDE");

  if(button_up || button_down)
  {
    if(!updn_pause)
    {
      updn_count = 0;
      updn_pause = true;
      if(button_up)
        temp_req += 0.5;
      if(button_down)
        temp_req -= 0.5;

      temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);
      //io.SHOW(12);
      io.SHOW(13);

      Serial.print("UP/DOWN CHANGED: ");
      Serial.println(temp_req);
    }
  }
  
  if(updn_pause && !button_up && !button_down)
  {
    updn_count = 0;
    updn_pause = false;
    Serial.println("UP/DOWN RELEASED");
  }

  if(updn_count >= LOOP_HOLD*LOOP_HOLD)
  {
    updn_request = false;
    seq_count = DISP_BASE_DELAY_LONG;

    save_eeprom = true;

    Serial.println("UP/DOWN DOWN");
  }

  Serial.print("UP/DOWN COUNT: ");
  Serial.println(updn_count);
  updn_count++;
}

void ioclass::BUTTONHOLD()
{
  if(!hold_request)
    return;  

  hold_count++;

  Serial.print("HOLD: ");
  Serial.println(hold_count);

  if(hold_count < LOOP_HOLD)
    return;

  hold_done = true;

  Serial.println("HOLD DONE");
}


void ioclass::BUTTONDONE()
{
  if(!hold_done || hold_release || (hold_count > LOOP_HOLD))
    return;

  if(button_lock)
  {
    if(!disp_locked)
    {
      disp_locked = true; 
      Serial.println("LOCKED");
    }
    else
    {
      disp_locked = false; 
      Serial.println("UNLOCKED");
    }

    io.SHOW(9);
  }
  
  if(button_spa)
  {
    if(!spa_mode)
    {
      spa_mode = true; 
      Serial.println("SPA MODE");
    }
    else
    {
      spa_mode = false; 
      Serial.println("POOL MODE");
    }

    io.SHOW(10);
  }

  save_eeprom = true;
  
  seq_count = DISP_BASE_DELAY_SHRT;
  hold_release = true;
}

void ioclass::BUTTONLOCK()
{
  if(!disp_error || done_error)
    return;

  if(hold_count == 0)
    Serial.println("START DISPLAYING ERROR");

  if(hold_count <= (LOOP_TIME/250))
    hold_count++;

  Serial.println(hold_count);

  if(hold_count == (LOOP_TIME/250)+1)
  {
    done_error = true;
    Serial.println("DONE DISPLAYING ERROR");

    hold_release = true;
    seq_count = DISP_BASE_NOW;
  }
}

void ioclass::SHOW(int select)
{
  if(!hold_done && hold_count == 0 && select == DISP_HOLD)
  {
    OLED.CHAR(0, 1, "Please hold button...", 1, true);
    OLED.CHAR(0, 2, "HOLD ", 4, true);
  }

  if(select == 2)
  {
    OLED.CHAR(0, 1, "Tset:", 2, true);
    OLED.CHAR(115, 1, "F ", 2, true);
    
    OLED.CHAR(0, 3, "Tin :", 2, true);
    OLED.CHAR(115, 3, "F ", 2, true);
  
    OLED.CHAR(0, 5, "Tout:", 2, true);
    OLED.CHAR(115, 5, "F ", 2, true);

    heating_disp = false;
  }
  if(select == 3)
  {
    String Treq = String(temp_req,1);
    String Tin = String(temp_in_F,1);
    String Tout = String(temp_out_F,1);
  
    OLED.CHAR(60, 1, Treq, 2, false);
  
    OLED.CHAR(60, 3, Tin, 2, false);
  
    OLED.CHAR(60, 5, Tout, 2, false);
  }

  if(select == 4)
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

  if(select == 14)
  {
    if(!save_done)
      OLED.CHAR(0, 7, "SETTINGS SAVED!", 1, true);
    save_done = true;
  }

  if(select == DISP_HEAT)
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

  if(select == 7)
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
    
    seq_count = DISP_BASE_NOW;
  }

  if(select == 8)
  {
    OLED.CHAR(0, 1, "Buttons are locked", 1, true);
    OLED.CHAR(0, 2, "LOCK ", 4, true);
  }

  if(select == 9)
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
    seq_count = 93;
  }

  if(select == 10)
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
    seq_count = 93;
  }

  if(select == 11)
  {
    #ifdef ETHERNET_ENABLE
      OLED.CHAR(0, 0, "Date/Time wait...", 1, false);
    #else
      OLED.CHAR(0, 0, "Heat Pump System", 1, false);
    #endif
  }

  if(select == 12)
  {
    OLED.CHAR(0, 1, "Requested Temperature", 1, true);
  }

  if(select == 13)
  {
    OLED.CHAR(0, 2, String(temp_req,1)+" F ", 4, false);
  }
}

void ioclass::FAN()
{
  
}

void ioclass::PUMP()
{
  
}

void ioclass::SAVE_EEPROM()
{
  if(!save_done)
  {
    Serial.println("EEPROM NOT SAVED");
    return;
  }

  //Convert temperature to lower number since the maximum eeprom value is 255 and temp has to be multiplied by 10 to accept decimal  
  //Process same modification while restoring data  
  

  #ifdef EEPROM_ENABLED
    int temp_adj = (temp_req-TEMP_MIN)*10;
    int bit_write = 0;
    
    bitWrite(bit_write, 0, disp_locked)
    bitWrite(bit_write, 1, spa_mode)
    bitWrite(bit_write, 2, heat_enable)
    //bitWrite(bit_write, 3, spare1)
    //bitWrite(bit_write, 4, spare2)
    //bitWrite(bit_write, 5, spare3)
    //bitWrite(bit_write, 6, spare4)
    //bitWrite(bit_write, 7, spare5)

    EEPROM.write(0, byte(temp_adj));
    EEPROM.write(1, byte(0));
    Serial.println("EEPROM SAVED");
  #else
    Serial.println("EEPROM DISABLED");
  #endif
}

void ioclass::READ_EEPROM()
{
  #ifdef EEPROM_ENABLED
    //READ DATA FROM EEPROM
    //Temperature data is lowered since it cannot accept values larger than 255
    //Temperature has to be multplied by 10 to accept decimal
    //Process same modification while saving data 
    temp_req = TEMP_MIN + ((int)EEPROM.read(0)/10);
      
    int bit_read = (int)EEPROM.read(1);
  
    disp_locked = bitRead(bit_read, 0);
    spa_mode = bitRead(bit_read, 1);
    heat_enable = bitRead(bit_read, 2);
    //spare1 = bitRead(bit_read, 3);
    //spare2 = bitRead(bit_read, 4);
    //spare3 = bitRead(bit_read, 5);
    //spare4 = bitRead(bit_read, 6);
    //spare5 = bitRead(bit_read, 7);
  #else
    //DEFAULT VALUES
    temp_req = TEMP_MIN + 10;
    
    spa_mode = false;
    disp_locked = true;
    heat_enable = true;
    //spare1 = false;
    //spare2 = false;
  #endif

  temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);                     //Make sure temperature is between range
}

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

void ioclass::SETPINS()
{
  //Inputs
  pinMode(BUTTONS, INPUT);
  pinMode(TEMP_LP, INPUT);
  pinMode(TEMP_IN_WATER, INPUT);
  pinMode(TEMP_OUT_WATER, INPUT);
  pinMode(SWITCH_FLOW, INPUT);
  pinMode(SWITCH_HP, INPUT);
  pinMode(SWITCH_LP, INPUT);

  //Outputs
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_HEAT_HIGH, OUTPUT);
  pinMode(RELAY_HEAT_ON, OUTPUT);
}

ioclass io = ioclass();
