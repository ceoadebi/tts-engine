// src/audio/pcm.cpp
#include <iostream>
#include <cstdint>
#include <cmath>
#include "tts/audio/pcm.hpp"


std::int16_t floatToPCM16(float sample) {
    if (!std::isfinite(sample)) {
        return 0;
    }

    if (sample > 1.0f) {
        sample = 1.0f;
    }

    if (sample < -1.0f) {
        sample = -1.0f;
    }

    float scale{};
    
    if (sample < 0.0f) {
        scale = 32768.0f;
    } else {
        scale = 32767.0f;
    }

    float scaled = sample * scale;
    float rounded = std::round(scaled);

    return static_cast<std::int16_t>(rounded);
}
