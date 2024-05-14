#include <Arduino.h>

#ifndef MOTOR_H
#define MOTOR_H

/**
 * @brief Motor class to control a DC motor with a PWM signal and a direction pin
 * @param pwmPin The PWM pin connected to the motor driver
 * @param dirPin The direction pin connected to the motor driver
 * @param positiveDirection false if the motor direction is inverted, true otherwise
 * 
 */
class Motor {
public:
    Motor(int pwmPin, int dirPin, bool positiveDirection) 
        : pwmPin_(pwmPin), dirPin_(dirPin), positiveDirection_(positiveDirection) {
        pinMode(pwmPin_, OUTPUT);
        pinMode(dirPin_, OUTPUT);
    }

    void setSpeed(int speed) {
        // Ensure speed is within valid range (0-255)
        speed = constrain(speed, -255, 255);

        
        // Set the direction based on the sign of the speed
        bool direction = (speed >= 0);
        setDirection(direction);

        int absSpeed = abs(speed);

        // Map the speed to avoid dead zone
        int mappedSpeed = map(absSpeed, 0, 255, 50, 255);

        // Set the PWM output
        analogWrite(pwmPin_, abs(speed));
    }

    void setDirection(bool direction) {
        if(!positiveDirection_){
            direction = !direction;
        }
        digitalWrite(dirPin_, direction ? HIGH : LOW);
    }

private:
    int pwmPin_;
    int dirPin_;
    bool positiveDirection_;
};

#endif // MOTOR_H