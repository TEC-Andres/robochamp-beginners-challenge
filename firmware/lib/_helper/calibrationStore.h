#pragma once
#include "constants.h"

// Per-motor gain (FL, FR, RL, RR), saved in flash so it survives reboots.
struct Calibration {
    static constexpr int motorCount = 4;
    float gain[motorCount] = {1.0f, 1.0f, 1.0f, 1.0f};

    void load();
    void save() const;
};
