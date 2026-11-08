// Entrypoint: drive the robot from the phone / gamepad page.  pio run -e carController
#include <Arduino.h>
#include "robotServer.h"

RobotServer server(RobotServer::Mode::Drive);

void setup() {
    Serial.begin(115200);
    server.begin();
}

void loop() {
    server.update();
}
