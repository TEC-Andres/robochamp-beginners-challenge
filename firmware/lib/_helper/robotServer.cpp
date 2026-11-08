#include "robotServer.h"
#include "constants.h"
#include "webAssets.gen.h"
#include <WiFi.h>

namespace {
    constexpr float maxPwm = 255.0f;
}

RobotServer::RobotServer(Mode mode)
    : mode(mode),
      sensor(Constants::Sensor::s0, Constants::Sensor::s1, Constants::Sensor::s2, Constants::Sensor::s3, Constants::Sensor::out),
      led(Constants::Led::red, Constants::Led::green, Constants::Led::blue),
      http(Constants::Network::httpPort),
      ws(Constants::Network::wsPort) {}

void RobotServer::begin() {
    if (mode != Mode::Verify) {
        motors.begin();
        sensor.begin();
        led.begin();
        servo.begin();
        cal.load();
        motors.setGains(cal.gain);
    }

    WiFi.mode(WIFI_AP);
    WiFi.softAP(Constants::Network::ssid, Constants::Network::password, 1, 0, 4); // channel 1, visible, 4 clients
    WiFi.setSleep(false); // radio sleep makes replies to phones very slow
    Serial.printf("WiFi '%s' up, open http://%s/%s\n", Constants::Network::ssid, WiFi.softAPIP().toString().c_str(),
                  mode == Mode::Calibration ? "?cal" : "");

    // The controller page is software/ bundled into one gzipped response by tools/embedWeb.py.
    http.on("/", [this]() {
        http.sendHeader("Content-Encoding", "gzip");
        http.sendHeader("Cache-Control", "no-cache");
        http.send_P(200, "text/html", (PGM_P)webIndexGz, webIndexGzLen);
    });
    http.onNotFound([this]() { http.send(404, "text/plain", "not found"); }); // phones probe /generate_204 etc.
    http.begin();

    ws.begin();
    ws.onEvent([this](uint8_t c, WStype_t t, uint8_t* p, size_t l) { onEvent(c, t, p, l); });
}

void RobotServer::update() {
    http.handleClient();
    ws.loop();

    if (mode == Mode::Verify) {
        if (millis() - verifyWindowMs >= 1000) {
            if (verifyCount) Serial.printf("[verify] %d msg/s (expect ~25 while a stick is held)\n", verifyCount);
            verifyCount = 0;
            verifyWindowMs = millis();
        }
        return;
    }

    if (moving && millis() - lastCommandMs > Constants::Network::failsafeMs) {
        motors.stop();
        moving = false;
    }
}

void RobotServer::onEvent(uint8_t client, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            if (mode == Mode::Verify) Serial.printf("[verify] client %u connected\n", client);
            else sendGains(client);
            break;
        case WStype_DISCONNECTED:
            if (mode == Mode::Verify) {
                Serial.printf("[verify] client %u disconnected\n", client);
                break;
            }
            motors.stop();
            moving = false;
            break;
        case WStype_TEXT: {
            char msg[64];
            if (length >= sizeof(msg)) return;
            memcpy(msg, payload, length);
            msg[length] = '\0';
            handle(client, msg);
            break;
        }
        default:
            break;
    }
}

void RobotServer::handle(uint8_t client, const char* msg) {
    if (mode == Mode::Verify) {
        verifyCount++;
        Serial.printf("[verify] %s\n", msg);
        return;
    }
    if (mode == Mode::Calibration && msg[0] == 'D') return; // no driving while calibrating

    float a, b, c, d;
    int idx;
    char reply[48];

    if (sscanf(msg, "D,%f,%f", &a, &b) == 2) {
        drive(a, b);
    } else if (sscanf(msg, "M,%d,%f", &idx, &a) == 2 && idx >= 0 && idx < Calibration::motorCount) {
        // single-motor test for calibration; the others stay off
        float v[Calibration::motorCount] = {};
        v[idx] = constrain(a, -1.0f, 1.0f) * maxPwm;
        motors.setSpeed(v[0], v[1], v[2], v[3]);
        lastCommandMs = millis();
        moving = a != 0.0f;
    } else if (sscanf(msg, "V,%d", &idx) == 1) {
        servo.write(idx);
    } else if (strcmp(msg, "S") == 0) {
        motors.stop();
        moving = false;
    } else if (sscanf(msg, "G,%f,%f,%f,%f", &a, &b, &c, &d) == 4) {
        const float g[Calibration::motorCount] = {a, b, c, d};
        for (int i = 0; i < Calibration::motorCount; i++) cal.gain[i] = constrain(g[i], 0.5f, 1.5f);
        motors.setGains(cal.gain);
    } else if (strcmp(msg, "SAVE") == 0) {
        cal.save();
    } else if (strcmp(msg, "C") == 0) {
        const Constants::ColorsOnTrack color = sensor.readColor();
        led.setColor(color);
        snprintf(reply, sizeof(reply), "C,%d", (int)color);
        ws.sendTXT(client, reply);
    } else if (sscanf(msg, "L,%d", &idx) == 1 && idx >= 0 && idx <= (int)Constants::ColorsOnTrack::Gray) {
        snprintf(reply, sizeof(reply), "L,%d", sensor.learn((Constants::ColorsOnTrack)idx) ? 1 : 0);
        ws.sendTXT(client, reply);
    }
}

void RobotServer::drive(float y, float w) {
    y = constrain(y, -1.0f, 1.0f);
    w = constrain(w, -1.0f, 1.0f);

    float left = y + w, right = y - w; // skid steer
    const float peak = max(fabsf(left), fabsf(right));
    if (peak > 1.0f) {
        left /= peak;
        right /= peak;
    }

    motors.setSpeed(left * maxPwm, right * maxPwm, left * maxPwm, right * maxPwm);
    lastCommandMs = millis();
    moving = true;
}

void RobotServer::sendGains(int client) {
    char reply[64];
    snprintf(reply, sizeof(reply), "G,%.2f,%.2f,%.2f,%.2f", cal.gain[0], cal.gain[1], cal.gain[2], cal.gain[3]);
    ws.sendTXT(client, reply);
}
