#include "motor.h"
#include "myWebsocket.h"
#include "camera.h"
#include "control.h"


void textHandler(char *message);

void setup()
{
  Serial.begin(115200);

  init_wifi();
  init_websockets(textHandler);


  // init_camera(); // do this before the imu_setup because the camera uses the i2c bus


  imu_setup();

  init_camera_PID();
}

void loop()
{
  delay(1000);
  // send_image(); //todo: this should be in a task
  // Serial.println("loop");
}

// Function to handle the text message
void textHandler(char *message)
{
  int x, y;
  sscanf(message, "x: %d, y: %d ", &x, &y);

  // set the anglar velocity setpoint
  set_camera_PID_data(x,y);
}