#pragma once

namespace Constants {
    enum class RotationDirection { Left, Right };

    enum class ColorFilter { Red, Green, Blue, Clear };

    enum class ColorsOnTrack { Red, Yellow, Green, Blue, Gray };

    namespace Motor {
        struct Config {
            int forwardPin; // PWM here to go forward (other pin low)
            int reversePin; // PWM here to go backward (other pin low)
            float speedOffset;
        };

        // Yellow TT gear motors stall below ~30% duty; nonzero commands are scaled into [minPwm, 255].
        constexpr float minPwm = 80.0f;
        constexpr float rotateSpeed = 200.0f; // spin-in-place speed

        // Driver enable pins, held HIGH for the whole run.
        constexpr int enable1 = 14, enable2 = 27;

        // Two driver channels, one per side: the front and rear motor on a side share a channel.
        constexpr Config Left  = {25, 33, 0.0f}; // MOTOR_A forward, MOTOR_B reverse
        constexpr Config Right = {32, 4,  0.0f}; // MOTOR_C forward, MOTOR_D reverse
    }

    namespace Network {
        constexpr const char* ssid = "RoboChamp";
        constexpr const char* password = "robochamp26"; // WPA2 needs >= 8 chars
        constexpr int httpPort = 80;
        constexpr int wsPort = 81;
        constexpr unsigned long failsafeMs = 500; // stop if no command arrives for this long
    }

    // TCS3200: OUT on an input-only pin; S0-S3 on free output pins (not used by motors, LED, servo or strapping).
    namespace Sensor {
        constexpr int s0 = 13, s1 = 16, s2 = 17, s3 = 23, out = 34;
    }

    // Common-cathode RGB LED with resistors.
    namespace Led {
        constexpr int red = 18, green = 5, blue = 19;
    }

    // Servo (ESP32Servo): 50 Hz, pulse range in microseconds.
    namespace ServoConfig {
        constexpr int pin = 21;
        constexpr int hz = 50;
        constexpr int minUs = 500, maxUs = 2400;
        constexpr int startDeg = 0;   // position on boot
        constexpr int centerDeg = 90;
    }
}
