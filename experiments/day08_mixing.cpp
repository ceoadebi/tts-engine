#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/mixer.hpp"
#include "tts/audio/envelope.hpp"
#include "tts/audio/wav_writer.hpp"

#include <cstdint>
#include <iostream>
#include <span>

int main() {
    constexpr std::uint32_t sampleRate{48000};

    AudioBuffer inputA{sampleRate};
    AudioBuffer inputB{sampleRate};
    AudioBuffer output{sampleRate};

    generateSineWave(inputA, 440.0, 0.8, sampleRate);
    generateSineWave(inputB, 660.0, 0.8, sampleRate);
    
    constexpr float gainA{0.5f};
    constexpr float gainB{0.5f};
    bool success = mixAudio(output, inputA, inputB, gainA, gainB);
    if(!success) {
        std::cerr << "Failed to mix audio\n";
        return 1;
    }

    bool envSuccess = applyEnvelope(output, 0.1, 0.1, sampleRate);
    if(!envSuccess) {
        std::cerr << "Failed to apply envelope\n";
        return 1;
    }

    std::span<const float> samples{output.data(), output.size()};

    bool writeSuccess = writeWavPCM16("mixed_440_660.wav", samples, sampleRate, 1);
    if(!writeSuccess) {
        std::cerr << "Failed to write mixed WAV file\n";
        return 1;
    }

    return 0;

}
