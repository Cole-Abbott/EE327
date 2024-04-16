#include <Arduino.h>
#include "motor.h"

// Create two instances of the Motor class
Motor motor1(32, 33, true);  
Motor motor2(25, 26, false); 

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  // Nothing to do here
  // Ramp the speeds from -255 to 255
  for (int speed = -255; speed <= 255; speed++)
  {
    // motor1.setSpeed(speed);
    motor2.setSpeed(speed);
    delay(10); // Adjust the delay time as needed
  }
  delay(1000); // Delay for 1 second before reversing the motors
  Serial.println("Reversing motors");
  for (int speed = 255; speed >= -255; speed--)
  {
    motor1.setSpeed(speed);
    motor2.setSpeed(speed);
    delay(10); // Adjust the delay time as needed
  }
}
