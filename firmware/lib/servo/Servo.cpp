#include "Servo.h"

void RobotServo::begin() {
    using namespace Constants::ServoConfig;
    motor.setPeriodHertz(hz);
    motor.attach(pin, minUs, maxUs);
    write(startDeg);
}

void RobotServo::write(int degrees) {
    motor.write(constrain(degrees, 0, 180));
}
