#include "ledRGB.h"
#include "constants.h"

LEDRGB::LEDRGB(int redPin, int greenPin, int bluePin)
    : _redPin(redPin), _greenPin(greenPin), _bluePin(bluePin) {}

void LEDRGB::begin() {
    pinMode(_redPin, OUTPUT);
    pinMode(_greenPin, OUTPUT);
    pinMode(_bluePin, OUTPUT);
    // boot self-test: red, green, blue, off. If this shows but colors never change, the problem is the sensor, not the LED.
    const int test[][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 0, 0}};
    for (const auto& c : test) { setColor(c[0], c[1], c[2]); delay(300); }
}

void LEDRGB::setColor(int red, int green, int blue) {
    analogWrite(_redPin, red);
    analogWrite(_greenPin, green);
    analogWrite(_bluePin, blue);
}

void LEDRGB::setColor(Constants::ColorsOnTrack color) {
    switch (color) {
        case Constants::ColorsOnTrack::Red:    setColor(255, 0, 0); break;     // red
        case Constants::ColorsOnTrack::Yellow: setColor(255, 255, 0); break;   // yellow
        case Constants::ColorsOnTrack::Green:  setColor(0, 255, 0); break;     // green
        case Constants::ColorsOnTrack::Blue:   setColor(0, 0, 255); break;     // blue
        case Constants::ColorsOnTrack::Gray:   setColor(128, 128, 128); break; // gray
    }
    
}