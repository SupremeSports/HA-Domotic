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

//READ BUTTONS
//  This function reads buttons analog input
//  Sets a boolean variable according to pressed button for easy usage
//  Voltage ranges are +/- 10 of nominal value to allow small value change due to temperature, component wear, etc.
//  Function is called periodically in the sequence but also by subfunctions of this class to validate if a button is still pressed
void userclass::read_buttons()
{
  button = (int)analogRead(BUTTONS);

  //Convert analog buttons to actual boolean buttons
  button_lock = (button >= LK_MIN && button <= LK_MAX);
  button_up   = (button >= UP_MIN && button <= UP_MAX);
  button_down = (button >= DN_MIN && button <= DN_MAX);
  button_mode = (button >= MD_MIN && button <= MD_MAX);
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
  if(button < BUTTONS_OFF)
  {
    error_done = false;
    hold_done = false;
    
    return;
  }

  if(error_done || hold_done)
    return;

  prev_alarm = 0;

  show.on();
  
  if((button_mode || button_up || button_down) && disp_locked && !error_done)
    user.error();

  if((button_lock || (button_mode && !disp_locked)) && !hold_done)
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
  while(button > BUTTONS_OFF && !error_done);
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
  button_count = 0;

  show.hold();

  do
  {
    delay(LOOP_TIME);
    
    user.read_buttons();

    if(button_count > LOOP_HOLD)
      hold_done = true;

    button_count++;
  }
  while((button_lock || button_mode) && !hold_done);

  if(!hold_done)
    return;
  else
  {
    if(button_lock)
      user.lock();
    if(button_mode)
      user.mode();

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

//MODE SUBFUNCTION
//  This subfunction is used to toggle MODE status. It also displays the new status
void userclass::mode()
{
  spa_mode = spa_mode ? false : true;

  show.mode();
}

userclass user = userclass();
