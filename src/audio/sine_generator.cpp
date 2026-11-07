// src/audio/sine_generator.cpp
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/audio_buffer.hpp"
#include <cmath>
#include <cstdint>
#include <numbers>


void generateSineWave(
    AudioBuffer& buffer,
    double frequency,
    double amplitude,
    std::uint32_t sampleRate
) {
    if (sampleRate == 0 ||
        !std::isfinite(frequency) ||
        !std::isfinite(amplitude) ||
        frequency <= 0.0 ||
        frequency >= static_cast<double>(sampleRate) / 2.0 ||
        amplitude < 0.0 ||
        amplitude > 1.0) {
        return;
    }
    
    for (std::size_t i{0}; i < buffer.size(); ++i) {
         const double t = static_cast<double>(i) / sampleRate;

         const double phase = 2.0 * std::numbers::pi * frequency * t;

         const float sample = static_cast<float>(amplitude * std::sin(phase));

         buffer.data()[i] = sample;
    }
} 

