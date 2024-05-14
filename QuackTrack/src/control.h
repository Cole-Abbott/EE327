#include <Arduino.h>

void imu_setup();
void init_camera_PID();
void set_camera_PID_data(int x, int y);
void set_imu_setpoint(float angleSetpoint, float speedSetpoint);

