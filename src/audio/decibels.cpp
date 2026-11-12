// src/tts/decibels.cpp
#include "tts/audio/decibels.hpp" //decibels relative to full scale
#include <cstdint>
#include <cmath>
#include <limits>

double amplitudeToDbFS(double amplitude) {
    if (!std::isfinite(amplitude) || amplitude < 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    if (amplitude == 0.0) {
        return -std::numeric_limits<double>::infinity();
    }

    double result = 20.0 * std::log10(amplitude);
    return result;
}

double dbFSToAmplitude(double db) {
    if (std::isinf(db) && db < 0.0) {
        return 0.0;
    }

    if ((std::isinf(db) && db > 0.0) || std::isnan(db)) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    double amplitude = std::pow(10.0, db/20.0);
    if (!std::isfinite(amplitude)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return amplitude;
}
