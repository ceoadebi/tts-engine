
#include "tts/audio/analyzer.hpp"
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/decibels.hpp"
#include "tts/audio/envelope.hpp"
#include "tts/audio/mixer.hpp"
#include "tts/audio/normalizer.hpp"
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/wav_writer.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <span>

int main() {
    constexpr std::uint32_t sampleRate = 48000;
    constexpr std::size_t sampleCount = 48000;

    // Create audio buffers.
    AudioBuffer toneA(sampleCount);
    AudioBuffer toneB(sampleCount);
    AudioBuffer output(sampleCount);

    // Generate two sine waves.
    generateSineWave(toneA, 440.0, 0.8, sampleRate);
    generateSineWave(toneB, 660.0, 0.8, sampleRate);

    // Mix the two signals.
    if (!mixAudio(output, toneA, toneB, 0.5f, 0.5f)) {
        std::cerr << "Audio mixing failed\n";
        return 1;
    }

    // Apply attack and release.
    if (!applyEnvelope(output, 0.1, 0.1, sampleRate)) {
        std::cerr << "Envelope processing failed\n";
        return 1;
    }

    // Normalize the mixed signal.
    if (!normalizePeak(output, 0.9f)) {
        std::cerr << "Normalization failed\n";
        return 1;
    }

    // Analyze the floating-point samples.
    float minSample = 0.0f;
    float maxSample = 0.0f;
    float rms = 0.0f;

    if (output.size() >
        static_cast<std::size_t>(
            std::numeric_limits<int>::max())) {
        std::cerr << "Too many samples for analyzer\n";
        return 1;
    }

    const int count = static_cast<int>(output.size());

    if (!findMinSample(output.data(), count, &minSample) ||
        !findMaxSample(output.data(), count, &maxSample) ||
        !calculateRMS(output.data(), count, &rms)) {
        std::cerr << "Audio analysis failed\n";
        return 1;
    }

    // Peak must consider both positive and negative samples.
    const float peak = std::max(
        std::abs(minSample),
        std::abs(maxSample)
    );

    // Convert linear amplitudes into dBFS.
    const double peakDbFS = amplitudeToDbFS(peak);
    const double rmsDbFS = amplitudeToDbFS(rms);

    // Display measurements.
    std::cout << "\n=== Day 10: Analyzer Integration ===\n";
    std::cout << "Minimum sample: " << minSample << '\n';
    std::cout << "Maximum sample: " << maxSample << '\n';
    std::cout << "Peak amplitude: " << peak << '\n';
    std::cout << "RMS amplitude:  " << rms << '\n';
    std::cout << "Peak level:     " << peakDbFS << " dBFS\n";
    std::cout << "RMS level:      " << rmsDbFS << " dBFS\n";

    // Export the processed audio.
    const std::span<const float> samples(
        output.data(),
        output.size()
    );

    if (!writeWavPCM16(
            "day10_analyzed_audio.wav",
            samples,
            sampleRate,
            1)) {
        std::cerr << "WAV export failed\n";
        return 1;
    }

    std::cout << "WAV export successful\n";

    return 0;
}

