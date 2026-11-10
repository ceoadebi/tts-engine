// tts/audio/mixer.hpp
#ifndef TTS_AUDIO_MIXER_HPP
#define TTS_AUDIO_MIXER_HPP

#include "tts/audio/audio_buffer.hpp"

bool mixAudio(
    AudioBuffer& output,
    const AudioBuffer& inputA,
    const AudioBuffer& inputB,
    float gainA,
    float gainB
);

#endif
