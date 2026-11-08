#include "motor.h"
#include "constants.h"
#include <Arduino.h>

// Pins are configured in begin(), not the constructor: global Motor objects are
// constructed before the Arduino runtime is up on ESP32.
void Motor::begin() {
    pinMode(cfg.forwardPin, OUTPUT);
    pinMode(cfg.reversePin, OUTPUT);
    stop();
}

void Motor::setSpeed(float speed) {
    if (speed > 0.0f) {
        speed += cfg.speedOffset;
    } else if (speed < 0.0f) {
        speed -= cfg.speedOffset;
    }

    speed = constrain(speed * gain, -255.0f, 255.0f);

    if (fabsf(speed) < 1.0f) {
        stop();
        return;
    }

    // map 1..255 onto minPwm..255 so any commanded speed actually turns the motor
    const int pwm = (int)(Constants::Motor::minPwm + (fabsf(speed) - 1.0f) * (255.0f - Constants::Motor::minPwm) / 254.0f);

    analogWrite(speed > 0 ? cfg.reversePin : cfg.forwardPin, 0);
    analogWrite(speed > 0 ? cfg.forwardPin : cfg.reversePin, pwm);
}

void Motor::stop() {
    analogWrite(cfg.forwardPin, 0);
    analogWrite(cfg.reversePin, 0);
}
