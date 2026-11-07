#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/wav_writer.hpp"

#include <cstdint>
#include <iostream>
#include <span>

int main() {
    constexpr std::uint32_t sampleRate{48000};
    constexpr double frequency{440.0};
    constexpr double amplitude{0.8};

    AudioBuffer buffer{sampleRate};

    generateSineWave(buffer, frequency, amplitude, sampleRate);

    std::span<const float> samples{buffer.data(), buffer.size()};

    const bool success = writeWavPCM16("sine_440hz.wav", samples, sampleRate, 1);
    if(!success) {
        std::cerr << "Failed to write WAV file\n";
        return 1;
    }

    std::cout << "Generated sine_440hz.wav successfully\n";

    return 0;
}
