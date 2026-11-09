
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/envelope.hpp"
#include "tts/audio/wav_writer.hpp"

#include <cstdint>
#include <iostream>
#include <span>

int main() {
    constexpr std::uint32_t sampleRate{48000};
    constexpr double frequency{440.0};
    constexpr double amplitude{0.8};

    constexpr double attackSeconds{0.2};
    constexpr double releaseSeconds{0.2};

    // 1. Allocate one second of audio.
    AudioBuffer buffer{sampleRate};

    // 2. Generate a 440 Hz sine wave.
    generateSineWave(
        buffer,
        frequency,
        amplitude,
        sampleRate
    );

    // 3. Apply the amplitude envelope.
    const bool envelopeSuccess = applyEnvelope(
        buffer,
        attackSeconds,
        releaseSeconds,
        sampleRate
    );

    if (!envelopeSuccess) {
        std::cerr << "Failed to apply audio envelope\n";
        return 1;
    }

    // 4. Create a read-only view of the audio samples.
    const std::span<const float> samples{
        buffer.data(),
        buffer.size()
    };

    // 5. Write the audio to a PCM16 WAV file.
    const bool writeSuccess = writeWavPCM16(
        "sine_440hz_envelope.wav",
        samples,
        sampleRate,
        1
    );

    if (!writeSuccess) {
        std::cerr << "Failed to write WAV file\n";
        return 1;
    }

    std::cout << "Successfully generated sine_440hz_envelope.wav\n";
    std::cout << "Frequency: " << frequency << " Hz\n";
    std::cout << "Attack: " << attackSeconds << " seconds\n";
    std::cout << "Release: " << releaseSeconds << " seconds\n";

    return 0;
}

