#include "arduino.h"
#include "defines.h"
#include "variables.h"

#include "show.h"
#include "user.h"

bool  button_lock;   // Lock button status
bool  button_up;     // Up button status
bool  button_down;   // Down button status

//Button control timers
bool  error_done;
bool  hold_done;
int   button_count;

userclass::userclass(){}

//READ BUTTONS
//  This function reads buttons digital input
//  Function is called periodically in the sequence but also by subfunctions of this class to validate if a button is still pressed
void userclass::read_buttons()
{
  int multiple = 0;
  
  button_up   = !digitalRead(BUTTON_UP);
  button_down = !digitalRead(BUTTON_DN);
  button_lock = !digitalRead(BUTTON_LK);

  if(button_up)
    multiple++;
  if(button_down)
    multiple++;
  if(button_lock)
    multiple++;

  if(multiple > 1)
  {
    button_up = false;
    button_down = false;
    button_lock = false;
  }
}

//BUTTON WORK
//  This function controls the operations that are processed depending on pressed button
//  It controls the following function:
//  - Button error (pressing on buttons when they are locked)
//  - Button holding (LOCK and MODE buttons)
//  - Button up/down temperature request
//  If no button is pressed, 
void userclass::button_work()
{
  if(!button_up && !button_down && !button_lock)
  {
    error_done = false;
    hold_done = false;
    
    return;
  }

  if(error_done || hold_done)
    return;

  show.on();
  
  if((button_up || button_down) && disp_locked && !error_done)
    user.error();

  if(button_lock && !hold_done)
    user.hold();

  if((button_up || button_down) && !hold_done && !disp_locked)
    user.updown();

  seq_count = DISP_BASE_NOW;
}

//BUTTON ERROR
//  This function is called by user.button_work() whenever a button is pressed in lock mode (excluding lock button)
//  Function is time based on LOOP_TIME
//  It displays the error and keep it on screen until time base is done
//  Function exits after delay is done (and won't come back until button is depressed) or when button is depressed
void userclass::error()
{
  button_count = 0;

  show.error();
  
  do
  {
    delay(LOOP_TIME);
  
    user.read_buttons();

    if(button_count > LOOP_HOLD)
      error_done = true;
    
    button_count++;
  }
  while((button_up || button_down) && !error_done);
}

void userclass::updown()
{
  button_count = 0;
  
  delay(LOOP_TIME);
  
  show.temp_chg_base();

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

    show.temp_chg();

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

  save_eeprom = true;
}

//BUTTON HOLD
//  This function is called by user.button_work() whenever LOCK or MODE button is pressed
//  Function is time based on LOOP_TIME
//  It displays HOLD message and keep it on screen until time base is done
//  - Function exits if button is depressed
//  Once hold delay has been completed, function calls correct subfunction for LOCK or SPA as per selection
//  - Subfunction displays correct information then exits after DISP_HOLD delay
//  - Won't come back in function until button is depressed
void userclass::hold()
{
  hold_done = false;
  button_count = 0;

  show.hold();

  do
  {
    user.read_buttons();

    if(button_count > LOOP_HOLD)
      hold_done = true;

    button_count++;
    delay(LOOP_TIME);
  }
  while(button_lock && !hold_done);

  if(!hold_done)
    return;
  else
  {
    user.lock();

    delay(DISP_HOLD);
    save_eeprom = true;
  }
}

//LOCK SUBFUNCTION
//  This subfunction is used to toggle LOCK status. It also displays the new status
void userclass::lock()
{
  disp_locked = disp_locked ? false : true;
    
  show.lock();
}
userclass user = userclass();
