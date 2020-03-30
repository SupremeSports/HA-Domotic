#include "arduino.h"
#include "defines.h"
#include "variables.h"
#include <Mux.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "show.h"
#include "io.h"
#include "set.h"

long  prevMillis_disp;

Mux mux;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

ioclass::ioclass(){}

//SEQUENCE ROUTINE
//  This function runs once per loop cycle
//  It is used to slow down processes that needs to be visible for user on the OLED display
void ioclass::sequence()
{
  if(seq_count >= DISP_MAX)
  {
    /*if((millis() - prevMillis_disp) > DISP_OFF_DELAY)
      show.off();*/

    save_eeprom = false;
    
    seq_count = DISP_RUN;
  }
  
  if(seq_count == DISP_BASE_NOW)
  {
    show.base(0);
    
    if(save_eeprom)
    {
      if(disp_on)
        show.saved();
        
      set.eeprom_write();
    }

    prevMillis_disp = millis();

    io.read_sensors();      //Read sensors and determine alarms
    seq_count = DISP_RUN;   //Jump to normal sequence right away
  }

  if(seq_count == DISP_RUN && disp_on)
    show.data_disp();       //Update data display only once per cycle

  if(seq_count >= DISP_RUN && !save_eeprom && disp_on)
    show.manauto();         //Update heating status at each step for visual effects

  seq_count++;
}

void ioclass::read_sensors()
{
  int reading[15];
  for(int i=0; i<15; i++)
    reading[i] = mux.read(i);
  
  float reading_temp[6];
  for(int i=0; i<6; i++)
    reading_temp[i] = (temp_ratio_ax2*reading[i]*reading[i]) + (temp_ratio_bx*reading[i]) + temp_ratio_c;
    
  Temp_Inlet  = reading_temp[Temp_Water_Inlet];
  Temp_Solar  = reading_temp[Temp_Water_Solar];
  Temp_Middle = reading_temp[Temp_Water_Mid];
  Temp_Heater = reading_temp[Temp_Water_Heater];
  Temp_Return = reading_temp[Temp_Water_Return];
  Temp_Roof   = reading_temp[Temp_Roof_Surface];

  Sun_Level = (sun_ratio_ax2*reading[Sensor_Sun]*reading[Sensor_Sun]) + (sun_ratio_bx*reading[Sensor_Sun]) + sun_ratio_c; 
  Raining = (reading[Sensor_Rain] > 1000) ? true : false;
  Water_Pres = (pres_ratio_ax2*reading[Sensor_Pressure]*reading[Sensor_Pressure]) + (pres_ratio_bx*reading[Sensor_Pressure]) + pres_ratio_c;

  //POTENTIOMETERS RATIOS
  //  Made for use with a 10k potentiometer with 1k resistor to ground and 1k resistor to 5V+
  //  When potentiometer value reads ground, consider switch to be on
  //  When display is locked, all values are kept as they were
  if(!disp_locked)
  {
    Sw_Mtr[0] = (reading[POT_Solar] > 975) ? false : true;
    Sw_Mtr[1] = (reading[POT_Heater] > 975) ? false : true;
    Sw_Mtr[2] = (reading[POT_Inlet] > 975) ? false : true;
    
    Pot_Mtr[0] = Sw_Mtr[0] ? map(reading[POT_Solar], 100, 919, 0, 100) : 0;    // 0% = Full bypass of solar panels / 100% = Full to solar panel
    Pot_Mtr[1] = Sw_Mtr[1] ? map(reading[POT_Heater], 100, 919, 0, 100) : 0;   // 0% = Full bypass of heatpump / 100% = Full to heatpump
    Pot_Mtr[2] = Sw_Mtr[2] ? map(reading[POT_Inlet], 100, 919, 0, 100) : 0;    // 0% = Full from skimmer / 50% = 50/50 of skimmer/drain / 100% = Full from drain
  }
}

//Calculate solar panel percentage for MOTOR #1
int ioclass::calc_solar()
{
  //temp_req = 88.5;
  Temp_Roof = 98;
  Temp_Inlet = 82;
  Sun_Level = 500;
  
  int delta_temp = Temp_Roof - Temp_Inlet;

  //Create hysteresis for temperature control
  //Stops solar panels when pool water gets 2°F above setpoint
  //Starts solar panels when pool water gets 0.2°F below setpoint
  float temp_req_hi = temp_req + 2.0;
  float temp_req_lo = temp_req - 0.2;
  bool  solar_run;
  if(Temp_Inlet <= temp_req_lo)
    solar_run = true;
  if(Temp_Inlet >= temp_req_hi)
    solar_run = false;

  //Solar control valve condition
  if(Sw_Mtr[Mtr_1])
    Percent_Mtr[Mtr_1] = Pot_Mtr[Mtr_1];
  else if(Sun_Level >= SOLAR_MIN && solar_run)    
    Percent_Mtr[Mtr_1] = (calc_solar_ax2*delta_temp*delta_temp) + (calc_solar_bx*delta_temp) + calc_solar_c;
  else
    Percent_Mtr[Mtr_1] = 0; //%

  //If solar is disabled
  if(!solar_enable)
    Percent_Mtr[Mtr_1] = 0; //%

  Percent_Mtr[Mtr_1] = constrain(Percent_Mtr[Mtr_1], 0, 100);

  //90°  = solar panels are at 0% usage (completly bypassed)
  //180° = solar panels are at 100% usage
  int Angle_Mtr1 = map(Percent_Mtr[Mtr_1], 0, 100, 90, 180);

  return Angle_Mtr1;
}

//Calculate heatpump percentage for MOTOR #2
int ioclass::calc_heatpump()
{
  //Create hysteresis for temperature control
  float temp_req_hi = heatpump_min;
  float temp_req_lo = heatpump_min-2;
  bool  heatpump_run;
  if(Temp_Inlet <= temp_req_lo)
    heatpump_run = false;
  if(Temp_Inlet >= temp_req_hi)
    heatpump_run = true;

  //Heatpump control valve condition
  if(Sw_Mtr[Mtr_2] && heatpump_enable)
    Percent_Mtr[Mtr_2] = Pot_Mtr[Mtr_2];
  else if(Temp_Inlet > 70 && heatpump_enable)
    Percent_Mtr[Mtr_2] = 100; //%
  else
    Percent_Mtr[Mtr_2] = 0; //%

  //If heatpump is disabled
  if(!heatpump_enable)
    Percent_Mtr[Mtr_2] = 0; //%
    
  //90°  = heatpump is at 100% usage
  //180° = heatpump is at 0% usage (completly bypassed)
  int Angle_Mtr2 = map(Percent_Mtr[Mtr_2], 0, 100, 180, 90);         
  
  return Angle_Mtr2;
}

//Calculate drain/skimmer percentage for MOTOR #3
int ioclass::calc_inlet()
{
  if(Sw_Mtr[Mtr_3])
    Percent_Mtr[Mtr_3] = Pot_Mtr[Mtr_3];
  else if(Water_Pres < 2)
  {
    Percent_Mtr[Mtr_3] = 95; //%
    //TODO - Add delay to turn off, change percent slowly...
  }
  else if(true)
    Percent_Mtr[Mtr_3] = 0; //TODO
  else
    Percent_Mtr[Mtr_3] = 0; //%

  //0°   = drain/skimmer are at 0/100%
  //90°  = drain/skimmer are at 50/50%
  //180° = drain/skimmer are at 100/0%
  int Angle_Mtr3 = map(Percent_Mtr[Mtr_3], 0, 100, 0, 180);
                                                             
  return Angle_Mtr3;
}

void ioclass::move_motors()
{
  io.I2Cselect(Motors_Board);                         //Select appropriate I2C channel
  
  int Angle_Mtr1 = constrain(io.calc_solar(), 90, 180);
  int Angle_Mtr2 = constrain(io.calc_heatpump(), 90, 180);;
  int Angle_Mtr3 = constrain(io.calc_inlet(), 0, 180);;

  if(Angle_Mtr1 != Prev_Angle_Mtr[Mtr_1])
  {
    pwm.setPWM(PWM_MTR1, 0, pulseWidth(Angle_Mtr1));
    Prev_Angle_Mtr[Mtr_1] = Angle_Mtr1;
  }
  
  if(Angle_Mtr2 != Prev_Angle_Mtr[Mtr_2])
  {
    pwm.setPWM(PWM_MTR2, 0, pulseWidth(Angle_Mtr2));
    Prev_Angle_Mtr[Mtr_2] = Angle_Mtr2;
  }
  
  if(Angle_Mtr3 != Prev_Angle_Mtr[Mtr_3])
  {
    pwm.setPWM(PWM_MTR3, 0, pulseWidth(Angle_Mtr3));
    Prev_Angle_Mtr[Mtr_3] = Angle_Mtr3;
  }
}

void ioclass::I2Cselect(uint8_t i) 
{
  if(i>7 || i<0) 
    return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

int ioclass::pulseWidth(int angle)
{
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);

  return analog_value;
}

void ioclass::set_mux()
{
  mux.setup(MUX_S0, MUX_S1, MUX_S2, MUX_S3, MUX_SIG); // initialise on setup
}

void ioclass::set_pwm()
{
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);
  yield();
}

ioclass io = ioclass();
