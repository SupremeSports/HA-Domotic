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

//SEQUENCE ROUTINE
//  This function runs once per loop cycle
//  It is used to slow down processes that needs to be visible for user on the OLED display
void ioclass::sequence()
{
  if(seq_count >= DISP_MAX || alarm_on)
  {
    if((millis() - prevMillis_disp) > DISP_OFF_DELAY)
      show.off();

    save_eeprom = false;

    if(alarm_on)
      return;
    
    seq_count = DISP_RUN;
  }
  
  if(seq_count == DISP_BASE_NOW)
  {
    if(disp_on)
    {      
      show.temps_base();
      show.modes();
      
      if(save_eeprom)
        show.saved();
    }

    if(save_eeprom)
      set.eeprom_write();

    prevMillis_disp = millis();
    
    seq_count = DISP_RUN;   //Jump to normal sequence right away
  }

  if(seq_count == DISP_RUN && disp_on)
    show.temps_disp();      //Update temperatures display only once per cycle

  if(seq_count >= DISP_RUN && !save_eeprom && disp_on)
    show.heating();         //Update heating status at each step for visual effects

  seq_count++;
}

//PUMP OUTPUT CONTROL
// This function controls the pump output
// Pump is ON after HEAT_ON_DELAY after power up
// Pump is ON when no alarm is present
// Pump is can come ON only if heat enable is active
//  Heat is disabled when requested temperature is set below TEMP_MIN+1
//  Heat can be disabled by ethernet communication
//  Heat can be enabled by raising requested temperature or by ethernet communication
//  Ethernet commnuication doesn't erase temperature setting if disabled/enabled by ethernet
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

  if((alarm_bits == 0) && ((millis() - prevMillis_heat) > HEAT_ON_DELAY) && heat_enable)
  {
    if(temp_in_F <= temp_req_lo)
      pump_out = true;
    if(temp_in_F >= temp_req_hi)
      pump_out = false;
  }
  else
    pump_out = false;
  
  digitalWrite(RELAY_HEAT, pump_out);
}

//FAN OUTPUT CONTROL
// This function controls the fan output
// NORMAL OUTPUT
//  Fan runs at power up before the pump starts running
//  Fan runs as soon a the pump runs
//  Fan continues to run for FAN_OFF_DELAY after pump stops
//  Fan runs whenever fan_high is activated
// HIGH SPEED OUTPUT
//  Fan runs high speed when heating unit is freezing (<=32°F)
//  Fan runs high speed when high pressure triggers
void ioclass::fan()
{
  fan_high = ((temp_lp_raw < temp_lp_low) || !high_pres || ((millis() - prevMillis_heat) < HEAT_ON_DELAY));
  
  if(pump_out || fan_high)
    fan_out = true;
  else
  {
    if(fan_out)
    {
      if(!fan_off_wait)
      {
        prevMillis_fan = millis();
        fan_off_wait = true;
      }
      else if((millis() - prevMillis_fan) > FAN_OFF_DELAY)
      {
        fan_out = false;
        fan_high = false;
        fan_off_wait = false;
      }
    }
  }

  digitalWrite(RELAY_FAN_ON, fan_out);
  digitalWrite(RELAY_FAN_HIGH, fan_high);
}

//READ SENSORS
//  This function is used to read sensors status or values
void ioclass::read_sensors()
{
  int temp_in;
  int temp_out;
  
  temp_in = (int)analogRead(TEMP_IN_WATER);
  temp_in_F = (ratio_ax2 * temp_in * temp_in) + (ratio_bx * temp_in) + ratio_c;
  
  temp_out = (int)analogRead(TEMP_OUT_WATER);
  temp_out_F = (ratio_ax2 * temp_out * temp_out) + (ratio_bx * temp_out) + ratio_c;

  //Safety devices
  flow = !digitalRead(SWITCH_FLOW);
  high_pres = !digitalRead(SWITCH_HP);
  low_pres = !digitalRead(SWITCH_LP);
  
  temp_lp_raw = 750;//analogRead(TEMP_LP); //TO BE DELETED

  io.alarms(temp_in, temp_out);
}

//ALARMS CONTROLS
//  This function detects any issues and set bits accordingly into a variable
//  - Using an int to make a bit selection (0-15 for 16 errors maximum)
//  - "alarm_bits" variable will be read by ethernet communication to show alarms
//  Local display shows only most important alarm in priority order
//  Ethernet communication is able to gather all alarms at once by requesting "status" (see UDP_Receive.h)
void ioclass::alarms(int temp_in, int temp_out)
{
  alarm_bits = 0;

  //Find all effective alarms
  if(!high_pres)                       //Insufficient water output of the circulation pump or defective HP protection
    bitSet(alarm_bits, ALARM_HP);
  if(!low_pres)                       //Insufficient gas or defective LP control
    bitSet(alarm_bits, ALARM_LP);
  if(temp_lp_raw < temp_lp_low)       //Outside temperature low (unit in defrost mode)
    bitSet(alarm_bits, ALARM_FS);
  if(temp_lp_raw < PROBE_SHORT)       //Suction temperature probe is shorted
    bitSet(alarm_bits, ALARM_DPC);
  if(temp_lp_raw > PROBE_OPEN)        //Suction temperature probe is opened
    bitSet(alarm_bits, ALARM_DPO);
  if(!flow)                           //No water circulation or flow switch defective
    bitSet(alarm_bits, ALARM_FLO);
/*  if(!heat_enable)                    //Temperature set point is lower than 70°F and/or forced off
    bitSet(alarm_bits, ALARM_OFF);*/
  if(temp_in > PROBE_OPEN)            //The water temperature probe circuit is opened
    bitSet(alarm_bits, ALARM_PO);
  if(temp_in < PROBE_SHORT)           //The water temperature probe circuit is shorted
    bitSet(alarm_bits, ALARM_PC);

  if(prev_alarm != alarm_bits)
  {
    if(!disp_on)
      show.on();
      
    //Display only first detected alarm in the list (priority ordered)
    if(bitRead(alarm_bits, ALARM_HP))
      show.alarm_hp();
    else if(bitRead(alarm_bits, ALARM_LP))
      show.alarm_lp();
    else if(bitRead(alarm_bits, ALARM_FS))
      show.alarm_fs();
    else if(bitRead(alarm_bits, ALARM_DPC))
      show.alarm_dpc();
    else if(bitRead(alarm_bits, ALARM_DPO))
      show.alarm_dpo();
    else if(bitRead(alarm_bits, ALARM_FLO))
      show.alarm_flo();
/*    else if(bitRead(alarm_bits, ALARM_OFF))
      show.alarm_off();*/
    else if(bitRead(alarm_bits, ALARM_PO))
      show.alarm_po();
    else if(bitRead(alarm_bits, ALARM_PC))
      show.alarm_pc();
      
    prev_alarm = alarm_bits;

    if(alarm_bits == 0)
      seq_count = DISP_BASE_NOW;
    else
      prevMillis_disp = millis();
  }

  if(alarm_on && (alarm_bits == 0))
    prevMillis_heat = millis();       //Set back heating delay once all alarms are off and alarm_on is still on
    
  alarm_on = (alarm_bits == 0) ? false : true;
}

ioclass io = ioclass();
