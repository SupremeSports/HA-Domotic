#include "arduino.h"
#include "defines.h"
#include "variables.h"

#include "show.h"
#include "io.h"
#include "set.h"

long  prevMillis_disp;
long  prevMillis_fan;
bool  fan_off_wait;

ioclass::ioclass(){}

//PUMP OUTPUT CONTROL
// This function controls the pump output
// Pump runs whenever 
void ioclass::pump()
{
  float temp_req_hi;
  float temp_req_lo;
  //Adjust min/max accordingly to spa/pool mode
  if(spa_mode)
  {
    temp_req_lo = temp_req - 0.2;
    temp_req_hi = temp_req + 1.0;
  }
  else
  {
    temp_req_lo = temp_req - 0.5;
    temp_req_hi = temp_req + 0.5;
  } 

  if(((temp_lp > temp_lp_low) && low_pres && high_pres && flow && heat_enable) || true)
  {
    if(temp_in_F <= temp_req_lo)
      pump_out = true;
    if(temp_in_F >= temp_req_hi)
      pump_out = false;
    
    alarm_on = false;

    digitalWrite(RELAY_HEAT, fan_out);
  }
}

//FAN OUTPUT CONTROL
// This function controls the fan output
// Fan runs as soon a the pump runs
// Fan continues to run for 30s after pump stops
void ioclass::fan()
{
  if(pump_out || (temp_lp < temp_lp_low))
  {
    fan_out = true;
    if(temp_lp < temp_lp_low)
      fan_high = true;
  }
  else
  {
    if(fan_out)
    {
      if(!fan_off_wait)
      {
        prevMillis_fan = millis();
        fan_off_wait = true;
      }
      else
      {
        if((millis() - prevMillis_fan) > FAN_OFF_DELAY)
        {
          fan_out = false;
          fan_high = false;
          fan_off_wait = false;
        }
      }
    }
  }

  digitalWrite(RELAY_FAN_ON, fan_out);
  digitalWrite(RELAY_FAN_HIGH, fan_high);
}

//ALARMS CONTROLS
//Using an int to make a bit selection (0-15 for 16 errors maximum)
//"alarm_bits" variable will be read by ethernet communication to show alarms
//Local display shows only most important alarm in priority order
void ioclass::alarms(int temp_in, int temp_out)
{
  alarm_bits = 0;

  //Find all effective alarms
  if(high_pres)                //Insufficient water output of the circulation pump or defective HP protection
    bitSet(alarm_bits, 0);
  if(!low_pres)                //Insufficient gas or defective LP control
    bitSet(alarm_bits, 1);
  if(temp_lp < temp_lp_low)    //Outside temperature low (unit in defrost mode)
    bitSet(alarm_bits, 2);
  if(temp_lp < 10)             //Suction temperature probe is shorted
    bitSet(alarm_bits, 3);
  if(temp_lp > 1020)           //Suction temperature probe is opened
    bitSet(alarm_bits, 4);
  if(!flow)                    //No water circulation or flow switch defective
    bitSet(alarm_bits, 5);
  if(!heat_enable)             //Temperature set point is lower than 70°F and/or forced off
    bitSet(alarm_bits, 6);
  if(temp_in > 1020)           //The water temperature probe circuit is opened
    bitSet(alarm_bits, 7);
  if(temp_in < 10)             //The water temperature probe circuit is shorted
    bitSet(alarm_bits, 8);

  alarm_bits = 0; //TO BE DELETED

  //Display only first detected alarm in the list (priority ordered)
  if(bitRead(alarm_bits, 0))
    show.alarm_hp();
  else if(bitRead(alarm_bits, 1))
    show.alarm_lp();
  else if(bitRead(alarm_bits, 2))
    show.alarm_fs();
  else if(bitRead(alarm_bits, 3))
    show.alarm_dpc();
  else if(bitRead(alarm_bits, 4))
    show.alarm_dpo();
  else if(bitRead(alarm_bits, 5))
    show.alarm_flo();
  else if(bitRead(alarm_bits, 6))
    show.alarm_off();
  else if(bitRead(alarm_bits, 7))
    show.alarm_po();
  else if(bitRead(alarm_bits, 8))
    show.alarm_pc();

  alarm_on = (alarm_bits == 0) ? false : true;

}

void ioclass::sequence()
{    
  if(seq_count == DISP_BASE_NOW)
  {
    if(disp_on)
    {      
      show.temps_base();
      show.modes(disp_locked, spa_mode);
      
      if(save_eeprom)
        show.saved(save_eeprom);
    }

    if(save_eeprom)
      set.eeprom_write();

    prevMillis_disp = millis();
    
    seq_count = DISP_RUN;   //Jump to normal sequence right away
  }

  if(seq_count == DISP_RUN)
  {
    if(disp_on)
      show.temps(temp_req, temp_in_F, temp_out_F, heat_enable);
  }

  if(seq_count >= DISP_RUN && !save_eeprom)
  {
    if(disp_on)
      show.heating(seq_count, heat_enable, pump_out, alarm_on);
  }

  seq_count++;

  if(seq_count >= DISP_MAX)
  {
    if(prevMillis_disp >= DISP_OFF_DELAY)
      disp_on = show.off();

    save_eeprom = false;
    
    seq_count = DISP_RUN;
  }

  delay(0);
}

void ioclass::read_sensors()
{
  int temp_in;
  int temp_out;
  
  temp_in = (int)analogRead(TEMP_IN_WATER);
  temp_in_F = (ratio_ax2 * temp_in * temp_in) + (ratio_bx * temp_in) + ratio_c;
  
  temp_out = (int)analogRead(TEMP_OUT_WATER);
  temp_out_F = (ratio_ax2 * temp_out * temp_out) + (ratio_bx * temp_out) + ratio_c;

  temp_out_F = 85.3; //TO BE DELETED

  //Safety devices
  flow = digitalRead(SWITCH_FLOW);
  high_pres = digitalRead(SWITCH_HP);
  low_pres = digitalRead(SWITCH_LP);

  flow = true; //TO BE DELETED
  high_pres = false; //TO BE DELETED
  low_pres = true; //TO BE DELETED
  
  temp_lp = (int)analogRead(TEMP_LP);

  temp_lp = 500; //TO BE DELETED

  io.alarms(temp_in, temp_out);
}

ioclass io = ioclass();
