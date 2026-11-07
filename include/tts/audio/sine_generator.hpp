// tts/audio/sine_generator.hpp
#ifndef TTS_AUDIO_SINE_GENERATOR_HPP
#define TTS_AUDIO_SINE_GENERATOR_HPP

#include "tts/audio/audio_buffer.hpp"
#include <cstdint>

void generateSineWave(
    AudioBuffer& buffer,
    double frequency,
    double amplitude,
    std::uint32_t sampleRate
);

#endif 
