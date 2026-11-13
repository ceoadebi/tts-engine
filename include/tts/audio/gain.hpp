// tts/audio/gain.hpp
#ifndef TTS_AUDIO_GAIN_HPP
#define TTS_AUDIO_GAIN_HPP

#include "tts/audio/audio_buffer.hpp"

bool applyGainDb(AudioBuffer& buffer, double gainDb);

#endif


