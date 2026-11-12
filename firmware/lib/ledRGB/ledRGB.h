#pragma once
#include <Arduino.h>
#include "constants.h"

class LEDRGB {
public:
    LEDRGB(int redPin, int greenPin, int bluePin);
    void begin();
    void setColor(int red, int green, int blue);
    void setColor(Constants::ColorsOnTrack color);

private:
    int _redPin, _greenPin, _bluePin;
    static void write(int pin, int value);
};

