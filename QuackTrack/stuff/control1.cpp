#include "control.h"
#include "motor.h"

// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>

#include <BitBang_I2C.h>

// Motor pins
// Create two instances of the Motor class
// Motor motorL(14, 15, true);
// Motor motorR(2, 12, true);

// Global variable to store the setpoint and the camera data
volatile float _global_imu_setpoint = 0;
volatile int _global_camera_data = 240;
volatile int _global_camera_flag = 0;

// I2C pins
#define SDA_PIN 14
#define SCL_PIN 1
#define MPU 0x68
BBI2C bbi2c;

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
    // use bitbang i2c to avoid conflicts with the camera
    memset(&bbi2c, 0, sizeof(bbi2c));
    bbi2c.bWire = false; // use bit bang, not wire library
    bbi2c.iSDA = SDA_PIN;
    bbi2c.iSCL = SCL_PIN;
    I2CInit(&bbi2c, 1000L); // slow down so Nscope can see it
    // I2CInit(&bbi2c, 100000L);
    delay(100); // allow devices to power up

    
    // write to the power management register to wake up the IMU
    uint8_t data[2] = {0x6B, 0};
    I2CWrite(&bbi2c, MPU, data, 2);

    // read the who am i register
    uint8_t whoAmI[1] = {0x75};
    uint8_t whoAmIData[1];
    int bitsWrote = I2CWrite(&bbi2c, MPU, whoAmI, 1);
    int bitsRead = I2CRead(&bbi2c, MPU, whoAmIData, 1);
    Serial.printf("wrote: %d, read: %d\n", bitsWrote, bitsRead);
    Serial.printf("Who am I: %d\n", whoAmIData[0]);

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

        // // read the gyro data
        // Wire.beginTransmission(MPU);
        // Wire.write(0x43); // starting with register 0x43 (GYRO_XOUT_H)
        // Wire.endTransmission(false);
        // Wire.requestFrom(MPU, 6, true);
        // int16_t x = Wire.read() << 8 | Wire.read();
        // int16_t y = Wire.read() << 8 | Wire.read();
        // int16_t z = Wire.read() << 8 | Wire.read();
        int16_t z = 0;

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
        // motorL.setSpeed((int)-result);
        // motorR.setSpeed((int)result);

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