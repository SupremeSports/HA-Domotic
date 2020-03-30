/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() 
{
  int offset = 5;
  int min_sweep = 0 + offset;
  int max_sweep = 180 + offset;
  int loop_delay = 1000;
  
  myservo.write(min_sweep);              // tell servo to go to position in variable 'pos'
  delay(loop_delay); 
  myservo.write(max_sweep);              // tell servo to go to position in variable 'pos'
  delay(loop_delay); 
  
}

