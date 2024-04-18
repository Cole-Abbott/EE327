#include "motor.h"
#include "myWebsocket.h"

void handleMessage(char *message);

// Create two instances of the Motor class
Motor motor1(32, 33, true);
Motor motor2(25, 26, true);

void setup()
{
  Serial.begin(115200);
  init_wifi();
  init_websockets(handleMessage);
}

void loop()
{
  delay(1000);
}

/**
 * @brief Handles the message received from the websocket
 *
 * @param message
 */ 
void handleMessage(char *message)
{
  if (strcmp(message, "forward") == 0)
  {
    motor1.setSpeed(255);
    motor2.setSpeed(255);
  }
  else if (strcmp(message, "backward") == 0)
  {
    motor1.setSpeed(-255);
    motor2.setSpeed(-255);
  }
  else if (strcmp(message, "left") == 0)
  {
    motor1.setSpeed(-255);
    motor2.setSpeed(-255);
  }
  else if (strcmp(message, "right") == 0)
  {
    motor1.setSpeed(255);
    motor2.setSpeed(-255);
  }
  else if (strcmp(message, "stop") == 0)
  {
    motor1.setSpeed(0);
    motor2.setSpeed(0);
  }
  else
  {
    Serial.println("Invalid command");
  }
}