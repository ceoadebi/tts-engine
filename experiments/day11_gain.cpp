#include "tts/audio/analyzer.hpp"
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/decibels.hpp"
#include "tts/audio/envelope.hpp"
#include "tts/audio/gain.hpp"
#include "tts/audio/mixer.hpp"
#include "tts/audio/normalizer.hpp"
#include "tts/audio/sine_generator.hpp"
#include "tts/audio/wav_writer.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <span>

namespace {

struct Levels {
    float peak{};
    float rms{};
};

bool measureLevels(const AudioBuffer& buffer, Levels& levels) {
    if (buffer.size() == 0 ||
        buffer.size() >
            static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        return false;
    }

    const int count = static_cast<int>(buffer.size());
    float minimum{};
    float maximum{};
    float rms{};

    if (!findMinSample(buffer.data(), count, &minimum) ||
        !findMaxSample(buffer.data(), count, &maximum) ||
        !calculateRMS(buffer.data(), count, &rms)) {
        return false;
    }

    levels.peak = std::max(std::abs(minimum), std::abs(maximum));
    levels.rms = rms;
    return true;
}

} // namespace

int main() {
    constexpr std::uint32_t sampleRate{48000};
    constexpr std::size_t sampleCount{48000};
    constexpr double gainDb{-6.0206};

    AudioBuffer toneA{sampleCount};
    AudioBuffer toneB{sampleCount};
    AudioBuffer output{sampleCount};

    // These functions return void.
    generateSineWave(toneA, 440.0, 0.8, sampleRate);
    generateSineWave(toneB, 660.0, 0.8, sampleRate);

    if (!mixAudio(output, toneA, toneB, 0.5f, 0.5f) ||
        !applyEnvelope(output, 0.1, 0.1, sampleRate) ||
        !normalizePeak(output, 0.9f)) {
        std::cerr << "Failed to prepare the baseline audio\n";
        return 1;
    }

    const std::size_t samplesBefore = output.size();

    Levels before{};
    if (!measureLevels(output, before)) {
        std::cerr << "Could not measure levels before gain\n";
        return 1;
    }

    if (!applyGainDb(output, gainDb)) {
        std::cerr << "Could not apply gain\n";
        return 1;
    }

    Levels after{};
    if (!measureLevels(output, after)) {
        std::cerr << "Could not measure levels after gain\n";
        return 1;
    }

    const double peakChangeDb =
        amplitudeToDbFS(after.peak) - amplitudeToDbFS(before.peak);
    const double rmsChangeDb =
        amplitudeToDbFS(after.rms) - amplitudeToDbFS(before.rms);

    std::cout << std::fixed << std::setprecision(6)
              << "Peak amplitude: " << before.peak << " -> " << after.peak << '\n'
              << "RMS amplitude:  " << before.rms << " -> " << after.rms << '\n'
              << "Peak change:    " << peakChangeDb << " dB\n"
              << "RMS change:     " << rmsChangeDb << " dB\n"
              << "Sample count:   " << samplesBefore
              << " -> " << output.size() << '\n';

    constexpr double toleranceDb{0.001};
    const bool levelsMatch =
        std::abs(peakChangeDb - gainDb) <= toleranceDb &&
        std::abs(rmsChangeDb - gainDb) <= toleranceDb;

    if (!levelsMatch || output.size() != samplesBefore) {
        std::cerr << "Day 11 benchmark failed\n";
        return 1;
    }

    const std::span<const float> samples{output.data(), output.size()};

    if (!writeWavPCM16("day11_attenuated.wav", samples, sampleRate, 1)) {
        std::cerr << "Could not write WAV file\n";
        return 1;
    }

    std::cout << "Day 11 benchmark passed. Wrote day11_attenuated.wav\n";
    return 0;
}
