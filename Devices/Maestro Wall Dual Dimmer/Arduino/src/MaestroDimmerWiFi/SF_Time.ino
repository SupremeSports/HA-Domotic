// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- TIME FUNCTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateTime()
{
  ONS_1s = false;
  ONS_5s = false;
  ONS_10s = false;
  ONS_1m = false;
  
  if (second() == Second)
    return;
  
  Second = second();

  ONS_1s = true;
  ONS_5s = Second%5==0;
  ONS_10s = Second%10==0;

  PRE_5s = (Second+1)%5==0 || ONS_5s; //Less than a second before next trigger

  if (minute() != Minute)
    ONS_1m = true;

  Minute = minute();
}
