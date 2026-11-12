#pragma once
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "calibrationStore.h"
#include "ledRGB.h"
#include "Servo.h"
#include "motors.h"
#include "TCS34725.h"

// WiFi access point that serves software/ from flash and takes commands over WebSocket.
//   phone -> robot: D,y,w | S | M,idx,v | G,fl,fr,rl,rr | SAVE | C | L,colorIdx | V,degrees
//   robot -> phone: G,fl,fr,rl,rr | C,colorIdx | L,0/1
// Shared by the three entrypoints in src/:
//   Drive        - everything (carController.cpp)
//   Calibration  - same, but D (drive) is ignored (calibration.cpp)
//   Verify       - touches no hardware, only logs incoming messages to Serial (controllerVerification.cpp)
class RobotServer {
public:
    enum class Mode { Drive, Calibration, Verify };

    explicit RobotServer(Mode mode);
    void begin();
    void update(); // call every loop()

private:
    const Mode mode;
    Motors motors;
    Calibration cal;
    TCS34725 sensor;
    LEDRGB led;
    RobotServo servo;
    WebServer http;
    WebSocketsServer ws;
    unsigned long lastCommandMs = 0;
    unsigned long verifyWindowMs = 0;
    int verifyCount = 0;
    bool moving = false;

    void onEvent(uint8_t client, WStype_t type, uint8_t* payload, size_t length);
    void handle(uint8_t client, const char* msg);
    void drive(float y, float w); // both -1..1, w > 0 turns right
    void sendGains(int client);
};
