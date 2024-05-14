#include <Arduino.h>
#include "control.h"


#define TRAJ_LEN 50
#define TRAJ_AMPLITUDE 0.5
#define SPEED 100
float traj[TRAJ_LEN];



void setup() {
    Serial.begin(115200);
    
    imu_setup();

     for (int i = 0; i < TRAJ_LEN; i++)
    {
        traj[i] = TRAJ_AMPLITUDE * sin(PI * i / (TRAJ_LEN / 2));
    }


}



void loop() {

    static int i = 0;     
    set_imu_setpoint(traj[i], SPEED);
    i = (i + 1) % TRAJ_LEN;  
    delay(50);
   
}