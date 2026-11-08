#include "calibrationStore.h"
#include <Preferences.h>

namespace {
    const char* const keys[Calibration::motorCount] = {"g0", "g1", "g2", "g3"};
}

void Calibration::load() {
    Preferences p;
    p.begin("robot", true);
    for (int i = 0; i < motorCount; i++) gain[i] = p.getFloat(keys[i], 1.0f);
    p.end();
}

void Calibration::save() const {
    Preferences p;
    p.begin("robot", false);
    for (int i = 0; i < motorCount; i++) p.putFloat(keys[i], gain[i]);
    p.end();
}
