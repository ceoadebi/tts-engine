// src/audio/normalizer.cpp
#include "tts/audio/normalizer.hpp"
#include "tts/audio/audio_buffer.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

bool normalizePeak(
    AudioBuffer& buffer,
    float targetPeak
) {
    if (!std::isfinite(targetPeak) ||
        targetPeak < 0.0f ||
        targetPeak > 1.0f) {
    return false;
    }
    
    float peak {0.0f};

    for (std::size_t i{0}; i < buffer.size(); ++i) {
        const float sample = buffer.data()[i];

        if (!std::isfinite(sample)) {
            return false;
        }
        peak = std::max(peak, std::abs(sample));
    }
    if (peak == 0.0f) {
        return true;
    }
    const float gain = targetPeak / peak;
    for (std::size_t i{0}; i < buffer.size(); ++i) {
         buffer.data()[i] *= gain;
    }

    return true;
}
