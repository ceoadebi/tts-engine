// src/audio/gain.cpp
#include "tts/audio/gain.hpp"
#include "tts/audio/decibels.hpp"

#include <cmath>
#include <limits>
#include <cstddef>

bool applyGainDb(AudioBuffer& buffer, double gainDb) {
    const double multiplier = dbFSToAmplitude(gainDb);

    if (!std::isfinite(multiplier)) {
        return false;
    }

    constexpr double maxSample = std::numeric_limits<float>::max();

    for (std::size_t i{0}; i < buffer.size(); ++i) {
        const double scaled = static_cast<double>(buffer.data()[i]) * multiplier;

        if (!std::isfinite(scaled) || std::abs(scaled) > maxSample) {
            return false;
        }
    }

    for (std::size_t i{0}; i < buffer.size(); ++i) {
        const double scaled = static_cast<double>(buffer.data()[i]) * multiplier;

        buffer.data()[i] = static_cast<float>(scaled);
    }
    
    return true;
}
