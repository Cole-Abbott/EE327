#include "control.h"
#include "motor.h"

// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
#include <Wire.h>

// Motor pins
// Create two instances of the Motor class
Motor motorL(14, 15, true);
// Motor motorR(2, 4, true); 
Motor motorR(2, 12, true); 

// Global variable to store the setpoint and the camera data
volatile float _global_imu_setpoint = 0;
volatile int _global_camera_data = 240;
volatile int _global_camera_flag = 0;

// Create an instance of the MPU6050 class
// Adafruit_MPU6050 mpu;
#define SDA_PIN 13
#define SCL_PIN 16
#define MPU 0x68

// PID controller parameters
#define imu_Kp 350
#define imu_Ki 15
#define imu_Kd 0
#define imu_I_max 10

#define camera_Kp 1
#define camera_Ki 0
#define camera_Kd 0
#define camera_I_max 1000

void imu_loop_task(void *parameter);
void camera_PID_task(void *parameter);

void imu_setup()
{
    // use wire for the imu since wire1 is used for the camera
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.beginTransmission(MPU);

    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);
    Wire.endTransmission(true);
    Serial.println("done Wire");

    // // gyro config
    // Wire.beginTransmission(MPU);
    // Wire.write(0x1B);
    // Wire.write(0x10);
    // Wire.endTransmission(true);
    // // accelometer config
    // Wire.beginTransmission(MPU);
    // Wire.write(0x1C);
    // Wire.write(0x10);
    // Wire.endTransmission(true);

    // read the WHO_AM_I register
    Wire.beginTransmission(MPU);
    Wire.write(0x75); // WHO_AM_I register
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 1, true);
    int response = Wire.read();
    Serial.printf("response: %d\n", response);

    //     // setup the mp6050
    //     if (!mpu.begin())
    // {
    //     Serial.println("Failed to find MPU6050 chip");
    //     while (1)
    //     {
    //         delay(10);
    //     }
    // }

    // xTaskCreatePinnedToCore(
    //     imu_loop_task,   /* Function to implement the task */
    //     "imu_loop_task", /* Name of the task */
    //     10000,           /* Stack size in words */
    //     NULL,            /* Task input parameter */
    //     1,               /* Priority of the task */
    //     NULL,            /* Task handle. */
    //     1);              /* Core where the task should run */

    Serial.println("IMU setup done");
}

void imu_loop_task(void *parameter)
{

    float I = 0;
    float prevError = 0;
    int count = 0;
    // sensors_event_t a, g, temp;

    while (1)
    {

        // Read the accelerometer data
        // mpu.getEvent(&a, &g, &temp);

        // read the gyro data
        Wire.beginTransmission(MPU);
        Wire.write(0x43); // starting with register 0x43 (GYRO_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6, true);
        int16_t x = Wire.read() << 8 | Wire.read();
        int16_t y = Wire.read() << 8 | Wire.read();
        int16_t z = Wire.read() << 8 | Wire.read();

        // use PID controller to keep angular velocity around z axis at a set point

        // Set the speed of the motors based on the angular velocity around the z axis
        float setPoint = _global_imu_setpoint;

        // float error = setPoint - g.gyro.z;
        float error = setPoint - z;

        // Calculate the PID terms

        I += error;
        if (I > imu_I_max)
        {
            I = imu_I_max;
        }
        else if (I < -imu_I_max)
        {
            I = -imu_I_max;
        }

        float result = imu_Kp * error + imu_Ki * I + imu_Kd * (error - prevError);
        prevError = error;

        // Set the speed of the motors
        motorL.setSpeed((int)-result);
        motorR.setSpeed((int)result);

        // Serial.printf(">Setpoint: %f\n", setPoint);
        // Serial.printf(">Gyro: %d\n", z);
        // Serial.printf(">Error: %f\n>Result: %f\n", error, result);
        // Serial.printf(">I: %f\n", I);

        // delay(5);
    }
}

void set_imu_setpoint(float setpoint)
{
    _global_imu_setpoint = setpoint;
}

void init_camera_PID()
{

    // Create a task to run the camera PID controller
    xTaskCreatePinnedToCore(
        camera_PID_task,   /* Function to implement the task */
        "camera_PID_task", /* Name of the task */
        10000,             /* Stack size in words */
        NULL,              /* Task input parameter */
        1,                 /* Priority of the task */
        NULL,              /* Task handle. */
        0);                /* Core where the task should run */
}

void camera_PID_task(void *parameter)
{
    float I = 0;
    float prevError = 0;
    int count = 0;

    int setPoint = 240;

    while (1)
    {
        // wait for new setpoint
        while (!_global_camera_flag)
        {
            delay(10);
        }
        _global_camera_flag = 0;

        int error = setPoint - _global_camera_data;

        I += error;
        if (I > camera_I_max)
        {
            I = camera_I_max;
        }
        else if (I < -camera_I_max)
        {
            I = -camera_I_max;
        }

        float result = camera_Kp * error + camera_Ki * I + camera_Kd * (error - prevError);
        prevError = error;

        // Set the angular velocity setpoint
        set_imu_setpoint(result);

        Serial.printf(">Camera:%d\n >Error:%d\n, >Result:%f\n", _global_camera_data, error, result);
    }
}

void set_camera_PID_data(int data)
{
    _global_camera_data = data;
    _global_camera_flag = 1;
}