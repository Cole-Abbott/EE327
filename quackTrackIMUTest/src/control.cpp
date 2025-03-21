#include "control.h"
#include "motor.h"

// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
#include <Wire.h>

// Motor pins
Motor motorL(32, 33, true);
Motor motorR(25, 26, true);

// Global variable to store the setpoint and the camera data
volatile float _global_imu_angle_setpoint = 0;
volatile float _global_imu_speed_setpoint = 0;
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

#define camera_Kp 0.005
#define camera_Ki 0
#define camera_Kd 0
#define camera_I_max 1000

void imu_loop_task(void *parameter);
void camera_PID_task(void *parameter);

void imu_setup()
{
    //deinit both i2c buses
    // Wire.end();
    // Wire1.end();
    
    // use Wire for the imu since Wire is used for the camera
    // Wire.begin(SDA_PIN, SCL_PIN);
    Wire.begin(); // default SDA = 21, SCL = 22
    Wire.beginTransmission(MPU);

    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0);
    Wire.endTransmission(true);
    Serial.println("done Wire");

    // gyro config, set the gyro to 250 degrees per second by setting the FS_SEL to 0
    Wire.beginTransmission(MPU);
    Wire.write(0x1B);
    Wire.write(0x0); // 0x0 is 0b00000000, FS_SEL = 0
    Wire.endTransmission(true);
    // accelometer config
    Wire.beginTransmission(MPU);
    Wire.write(0x1C);
    Wire.write(0x10);
    Wire.endTransmission(true);

    // read the WHO_AM_I register
    Wire.beginTransmission(MPU);
    Wire.write(0x75); // WHO_AM_I register
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 1);
    int response = Wire.read();
    Serial.printf("response: %d\n", response);

    xTaskCreatePinnedToCore(
        imu_loop_task,   /* Function to implement the task */
        "imu_loop_task", /* Name of the task */
        10000,           /* Stack size in words */
        NULL,            /* Task input parameter */
        1,               /* Priority of the task */
        NULL,            /* Task handle. */
        1);              /* Core where the task should run */

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
        // read the gyro data
        Wire.beginTransmission(MPU);
        Wire.write(0x43); // starting with register 0x43 (GYRO_XOUT_H)
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 6);
        int16_t x = Wire.read() << 8 | Wire.read();
        int16_t y = Wire.read() << 8 | Wire.read();
        int16_t z = Wire.read() << 8 | Wire.read();

        // convert gyro data to rad/s
        float gyroZ = z * 250.0 / 32768.0 * 3.14159 / 180.0;


        // use PID controller to keep angular velocity around z axis at a set point

        // Set the speed of the motors based on the angular velocity around the z axis
        float angle_setpoint = _global_imu_angle_setpoint;

        // float error = setPoint - g.gyro.z;
        float error = angle_setpoint - gyroZ;

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

        float turnResult = imu_Kp * error + imu_Ki * I + imu_Kd * (error - prevError);
        prevError = error;



        // Calculate the speed of the motors
        int result_R = turnResult + _global_imu_speed_setpoint;
        int result_L = -turnResult + _global_imu_speed_setpoint;

        // Set the speed of the motors
        motorR.setSpeed(result_R);
        motorL.setSpeed(result_L);

        Serial.printf(">Setpoint: %f\n", angle_setpoint);
        Serial.printf(">Gyro: %f\n", gyroZ);
        Serial.printf(">Error: %f\n", error);
        Serial.printf(">I: %f\n", I);
        Serial.printf(">Result_R: %d\n", result_R);
        Serial.printf(">Result_L: %d\n", result_L);

        delay(5);
    }
}

void set_imu_setpoint(float angleSetpoint, float speedSetpoint)
{
    _global_imu_angle_setpoint = angleSetpoint;
    _global_imu_speed_setpoint = speedSetpoint;

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
        set_imu_setpoint(result, 0);

        Serial.printf(">Camera:%d\n>Error:%d\n>Result:%f\n", _global_camera_data, error, result);
    }
}

void set_camera_PID_data(int data)
{
    _global_camera_data = data;
    _global_camera_flag = 1;
}