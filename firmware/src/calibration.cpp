// Entrypoint: motor gain + color sensor calibration (page opens on the calibration screen; driving is disabled).
//   pio run -e calibration
#include <Arduino.h>
#include "robotServer.h"

RobotServer server(RobotServer::Mode::Calibration);

void setup() {
    Serial.begin(115200);
    server.begin();
}

void loop() {
    server.update();
}
