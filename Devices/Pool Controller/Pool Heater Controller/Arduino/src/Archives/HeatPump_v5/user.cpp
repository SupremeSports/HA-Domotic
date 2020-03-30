#include "arduino.h"
#include "defines.h"
#include "variables.h"

#include "show.h"
#include "user.h"

int   button;
  bool  button_lock;   // Lock button status
  bool  button_mode;   // Spa button status
  bool  button_up;     // Up button status
  bool  button_down;   // Down button status

//Button control timers
bool  error_done;
bool  hold_done;
int   button_count;

userclass::userclass(){}

void userclass::read_buttons()
{
  button = (int)analogRead(BUTTONS);

  //Convert analog buttons to actual boolean buttons
  button_lock = (button >= LK_MIN && button <= LK_MAX);
  button_up = (button >= UP_MIN && button <= UP_MAX);
  button_down = (button >= DN_MIN && button <= DN_MAX);
  button_mode = (button >= MD_MIN && button <= MD_MAX);
}

void userclass::button_work()
{
  if(button < 20)
  {
    error_done = false;
    hold_done = false;
    
    return;
  }

  disp_on = show.on();

  if(error_done || hold_done)
    return;
  
  if((button_mode || button_up || button_down) && disp_locked && !error_done)
    user.error();

  if((button_lock || (button_mode && !disp_locked)) && !hold_done)
    user.hold();

  if((button_up || button_down) && !hold_done && !disp_locked)
    user.updown();

  seq_count = DISP_BASE_NOW;  // TO VERIFY
}

void userclass::error()
{
  button_count = 0;
  #ifdef DEBUG_ENABLE
    Serial.println("DISP LOCKED");
  #endif

  show.error();
  
  do
  {
    #ifdef DEBUG_ENABLE
      Serial.println("ERROR HOLD");
    #endif
    
    delay(LOOP_TIME);
  
    user.read_buttons();

    if(button_count > LOOP_HOLD)
      error_done = true;
    
    button_count++;
  }
  while(button > 20 && !error_done);
  
  #ifdef DEBUG_ENABLE
    Serial.println("ERROR RELEASED");
  #endif
  
  //seq_count = DISP_BASE_NOW;  // TO VERIFY
}

void userclass::hold()
{
  button_count = 0;

  #ifdef DEBUG_ENABLE
    Serial.println("PLEASE HOLD");
  #endif

  show.hold();

  do
  {
    #ifdef DEBUG_ENABLE
      Serial.println("BUTTON HOLD");
    #endif
    
    delay(LOOP_TIME);
    
    user.read_buttons();

    if(button_count > LOOP_HOLD)
      hold_done = true;

    button_count++;
  }
  while((button_lock || button_mode) && !hold_done);

  if(hold_done && button_lock)
    user.lock();
  if(hold_done && button_mode)
    user.mode();

  //if(!hold_done)  // TO VERIFY
  //  seq_count = DISP_BASE_NOW;  // TO VERIFY
}

void userclass::updown()
{
  button_count = 0;
  #ifdef DEBUG_ENABLE
    Serial.println("UP/DOWN BUTTONS");
  #endif
  
  delay(LOOP_TIME);
  
  show.temp_req_base();

  do
  {    
    if(button_up)
    {
      temp_req += 0.5;
      button_count = 0;
      
      if(!heat_enable)
        temp_req = TEMP_MIN+1;
    }
    if(button_down)
    {
      temp_req -= 0.5;
      button_count = 0;
    }

    temp_req = constrain(temp_req, TEMP_MIN, TEMP_MAX);

    heat_enable = (temp_req < TEMP_MIN+1) ? false : true;

    show.temp_req(temp_req);

    while(button_up || button_down)
    {
      delay(LOOP_TIME);
      user.read_buttons();
      button_count++;

      if(button_count > LOOP_HOLD)
        break;

      if(!button_up && !button_down)
        button_count = 0;
    };

    if(button_count > LOOP_HOLD)
      hold_done = true;

    button_count++;

    user.read_buttons();

    wdt_reset();
  }
  while(!hold_done);

  delay(DISP_HOLD);
  //seq_count = DISP_BASE_NOW;  // TO VERIFY

  save_eeprom = true;
}

void userclass::lock()
{
  if(disp_locked)
    disp_locked = false; 
  else
    disp_locked = true; 
    
  show.lock(disp_locked);
  delay(DISP_HOLD);
  //seq_count = DISP_BASE_NOW;  // TO VERIFY

  save_eeprom = true;
}

void userclass::mode()
{
  if(spa_mode)
    spa_mode = false; 
  else
    spa_mode = true; 

  show.mode(spa_mode);
  delay(DISP_HOLD);
  //seq_count = DISP_BASE_NOW;  // TO VERIFY

  save_eeprom = true;
}

userclass user = userclass();
