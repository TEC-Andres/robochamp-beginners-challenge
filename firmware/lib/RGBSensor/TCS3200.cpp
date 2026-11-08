#include "TCS3200.h"
#include "constants.h"
#include <Arduino.h>

namespace {
    constexpr unsigned long pulseTimeoutUs = 30000;
}

TCS3200::TCS3200(int s0, int s1, int s2, int s3, int out)
    : _s0(s0), _s1(s1), _s2(s2), _s3(s3), _out(out) {}

void TCS3200::begin() {
    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
    pinMode(_out, INPUT);
    setFrequencyScaling(2); // 20%
    loadDefaultSamples();
}

void TCS3200::setFrequencyScaling(int scaling) {
    // S0,S1: LL = power down, LH = 2%, HL = 20%, HH = 100%
    digitalWrite(_s0, (scaling == 2 || scaling == 3) ? HIGH : LOW);
    digitalWrite(_s1, (scaling == 1 || scaling == 3) ? HIGH : LOW);
}

void TCS3200::setColorFilter(Constants::ColorFilter filter) {
    // S2,S3: LL = red, LH = blue, HL = clear, HH = green
    digitalWrite(_s2, (filter == Constants::ColorFilter::Clear || filter == Constants::ColorFilter::Green) ? HIGH : LOW);
    digitalWrite(_s3, (filter == Constants::ColorFilter::Blue || filter == Constants::ColorFilter::Green) ? HIGH : LOW);
}

// Output is a square wave: a shorter period means more light. Returns 1/period (0 on timeout).
float TCS3200::readChannel(Constants::ColorFilter filter) {
    setColorFilter(filter);
    delay(2); // let the filter settle
    unsigned long low = pulseIn(_out, LOW, pulseTimeoutUs);
    return low == 0 ? 0.0f : 1000.0f / low;
}

RGB TCS3200::readRGB() {
    return {readChannel(Constants::ColorFilter::Red), readChannel(Constants::ColorFilter::Green), readChannel(Constants::ColorFilter::Blue)};
}

// Brightness-independent features: each channel as a share of the total. False if the sensor read nothing.
bool TCS3200::readFeatures(float f[3]) {
    const RGB c = readRGB();
    const float total = c.r + c.g + c.b;
    if (total <= 0.0f) return false;
    f[0] = c.r / total;
    f[1] = c.g / total;
    f[2] = c.b / total;
    return true;
}

// shortcut: made-up starting samples, not measured; call learn() on the real track to replace their effect.
void TCS3200::loadDefaultSamples() {
    static const float seed[][3] = {
        {0.60f, 0.25f, 0.15f}, {0.55f, 0.28f, 0.17f}, {0.65f, 0.22f, 0.13f},  // red
        {0.42f, 0.42f, 0.16f}, {0.40f, 0.44f, 0.16f}, {0.44f, 0.40f, 0.16f},  // yellow
        {0.25f, 0.50f, 0.25f}, {0.28f, 0.47f, 0.25f}, {0.22f, 0.53f, 0.25f},  // green
        {0.20f, 0.30f, 0.50f}, {0.22f, 0.28f, 0.50f}, {0.18f, 0.32f, 0.50f},  // blue
        {0.33f, 0.34f, 0.33f}, {0.34f, 0.33f, 0.33f}, {0.32f, 0.35f, 0.33f},  // gray
    };
    for (size_t i = 0; i < sizeof(seed) / sizeof(seed[0]); i++)
        knn.add(seed[i][0], seed[i][1], seed[i][2], i / 3); // label = ColorsOnTrack index
}

// Stores the current reading as an example of `color`. False if the sensor read nothing or memory is full.
bool TCS3200::learn(Constants::ColorsOnTrack color) {
    float f[3];
    return readFeatures(f) && knn.add(f[0], f[1], f[2], (int)color);
}

Constants::ColorsOnTrack TCS3200::readColor() {
    float f[3];
    if (!readFeatures(f)) return Constants::ColorsOnTrack::Gray;
    return (Constants::ColorsOnTrack)knn.classify(f[0], f[1], f[2]);
}
