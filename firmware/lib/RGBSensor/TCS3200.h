#pragma once

#include "KNN.h"
#include "constants.h"

// Relative intensity per channel (higher = more of that color). Not calibrated units.
struct RGB {
    float r, g, b;
};

class TCS3200 {
public:
    TCS3200(int s0, int s1, int s2, int s3, int out);
    void begin();
    void setFrequencyScaling(int scaling); // 0 = off, 1 = 2%, 2 = 20%, 3 = 100%
    void setColorFilter(Constants::ColorFilter filter);
    RGB readRGB();
    Constants::ColorsOnTrack readColor();             // KNN classification
    bool learn(Constants::ColorsOnTrack color);       // add the current reading as a training example

private:
    int _s0, _s1, _s2, _s3, _out;
    KNN<64> knn;
    void loadDefaultSamples();
    float readChannel(Constants::ColorFilter filter);
    bool readFeatures(float f[3]);
};
