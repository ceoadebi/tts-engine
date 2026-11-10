// src/audio/mixer.cpp
#include "tts/audio/mixer.hpp"
#include <cmath>
#include <cstddef>

bool mixAudio( 
    AudioBuffer& output,
    const AudioBuffer& inputA,
    const AudioBuffer& inputB,
    float gainA,
    float gainB
) {
    if (output.size() != inputA.size() ||
        output.size() != inputB.size()) {
        return false;
    }
    
    if (!std::isfinite(gainA) || !std::isfinite(gainB)) {
        return false;
    }

    for (std::size_t i{0}; i < output.size(); ++i) {
        output.data()[i] = inputA.data()[i] * gainA + inputB.data()[i] * gainB;
    }

    return true;
}
