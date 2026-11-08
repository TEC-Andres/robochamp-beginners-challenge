// Entrypoint: verify the controller -> robot link with no hardware attached. Every WebSocket message is logged
// to Serial and the message rate is printed each second.  pio run -e controllerVerification
#include <Arduino.h>
#include "robotServer.h"

RobotServer server(RobotServer::Mode::Verify);

void setup() {
    Serial.begin(115200);
    server.begin();
}

void loop() {
    server.update();
}
