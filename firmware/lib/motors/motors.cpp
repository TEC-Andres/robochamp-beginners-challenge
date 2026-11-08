#include <Arduino.h>
#include "motors.h"
#include "constants.h"

using namespace Constants::Motor;

Motors::Motors() : left(Left), right(Right) {}

void Motors::begin() {
    pinMode(enable1, OUTPUT);
    pinMode(enable2, OUTPUT);
    digitalWrite(enable1, HIGH);
    digitalWrite(enable2, HIGH);
    left.begin();
    right.begin();
}

void Motors::setSpeed(float fl, float fr, float rl, float rr) {
    left.setSpeed((fl + rl) / 2);
    right.setSpeed((fr + rr) / 2);
}

void Motors::drive(float speed) {
    setSpeed(speed, speed, speed, speed);
}

void Motors::rotate(Constants::RotationDirection direction) {
    const float s = direction == Constants::RotationDirection::Left ? rotateSpeed : -rotateSpeed;
    setSpeed(-s, s, -s, s); // left side opposes right side
}

void Motors::setGains(const float g[4]) {
    left.setGain((g[0] + g[2]) / 2);
    right.setGain((g[1] + g[3]) / 2);
}

void Motors::stop() {
    setSpeed(0, 0, 0, 0);
}
