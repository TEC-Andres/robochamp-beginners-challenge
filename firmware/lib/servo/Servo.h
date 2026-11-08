#pragma once
#include "constants.h"
#include <ESP32Servo.h>

// Hobby servo on Constants::ServoConfig (pin, 50 Hz, pulse range).
// Robotservo due to -fpermissive compiler flag. 
class RobotServo {
public:
    void begin();
    void write(int degrees); // clamped to 0..180
    void center() { write(Constants::ServoConfig::centerDeg); }

private:
    Servo motor;
};
