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

  init_camera(); // do this before the imu_setup because the camera uses the i2c bus

  imu_setup();
  init_camera_PID();
}

void loop()
{
  delay(100); // do nothing
}

// Function to handle the text message
void textHandler(char *message)
{
  if (message[0] == 'x')
  {
    int x, y;
    sscanf(message, "x: %d, y: %d ", &x, &y);

    // set the anglar velocity setpoint
    set_camera_PID_data(x, y);
  } else if (message[0] == 'm')
  {
    int w, v;
    sscanf(message, "m: %d, %d ", &w, &v);
    
    // set angular velocity setpoint
    set_imu_setpoint(w, v);


  } else if (message[0] == 't')
  {
    //toggle mode
    toggle_mode();
  } else if (message[0] == 'n')
  {
    //no person detected
    set_camera_PID_data(120, 266);
  }
}