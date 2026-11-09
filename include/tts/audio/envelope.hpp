// tts/audio/envelope.hpp
#ifndef TTS_AUDIO_ENVELOPE_HPP
#define TTS_AUDIO_ENVELOPE_HPP

#include <cstdint>
#include "tts/audio/audio_buffer.hpp"

bool applyEnvelope(
    AudioBuffer& buffer,
    double attackSeconds,
    double releaseSeconds,
    std::uint32_t sampleRate
);

#endif




