// tts/audio/normalizer.hpp
#ifndef TTS_AUDIO_NORMALIZER_HPP
#define TTS_AUDIO_NORMALIZER_HPP

#include "tts/audio/audio_buffer.hpp"

bool normalizePeak(
    AudioBuffer& buffer,
    float targetPeak
);

#endif
