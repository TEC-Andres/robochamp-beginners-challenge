#pragma once
#include "motor.h"
#include "constants.h"

// Four-wheel skid steer on two driver channels: front and rear wheels of a side get the same command
// (the average of the two), so per-wheel differences are not possible.
class Motors {
public:
    Motors();
    void begin();
    void setSpeed(float frontLeft, float frontRight, float rearLeft, float rearRight);
    void drive(float speed);   // both sides, + forward / - backward
    void rotate(Constants::RotationDirection direction); // spin in place (skid steer)
    void stop();
    void setGains(const float gains[4]); // FL, FR, RL, RR (averaged per side)

private:
    Motor left;
    Motor right;
};
