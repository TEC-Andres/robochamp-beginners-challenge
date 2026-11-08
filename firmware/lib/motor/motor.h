#pragma once

#include "constants.h"

// One driver channel (a forward pin and a reverse pin).
class Motor {
public:
    explicit Motor(const Constants::Motor::Config& cfg) : cfg(cfg) {}
    void begin();
    void setSpeed(float speed); // -255..255, sign = direction
    void stop();
    void setGain(float g) { gain = g; } // calibration multiplier

private:
    Constants::Motor::Config cfg;
    float gain = 1.0f;
};
