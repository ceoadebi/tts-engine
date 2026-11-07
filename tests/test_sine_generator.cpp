
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/sine_generator.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numbers>

namespace {

int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, const char* description) {
    if (condition) {
        ++testsPassed;
        std::cout << "[PASS] " << description << '\n';
    } else {
        ++testsFailed;
        std::cerr << "[FAIL] " << description << '\n';
    }
}

bool approximatelyEqual(
    double actual,
    double expected,
    double tolerance = 1e-5
) {
    return std::abs(actual - expected) <= tolerance;
}

} // namespace

int main() {
    constexpr std::uint32_t sampleRate{48000};
    constexpr double frequency{440.0};
    constexpr double amplitude{0.8};

    // Test 1: Generate a one-second sine wave.
    AudioBuffer buffer{sampleRate};

    generateSineWave(
        buffer,
        frequency,
        amplitude,
        sampleRate
    );

    check(
        buffer.size() == 48000,
        "Buffer contains 48000 samples"
    );

    // Test 2: Verify the first sample.
    check(
        approximatelyEqual(buffer.data()[0], 0.0),
        "First sample is zero"
    );

    // Test 3: Verify individual waveform samples.
    for (std::size_t i : {1u, 100u, 1000u, 12000u}) {
        const double t =
            static_cast<double>(i) / sampleRate;

        const double expected =
            amplitude * std::sin(
                2.0 * std::numbers::pi * frequency * t
            );

        check(
            approximatelyEqual(buffer.data()[i], expected),
            "Generated sample matches sine equation"
        );
    }

    // Test 4: Verify amplitude limits.
    bool withinAmplitude = true;

    for (std::size_t i{0}; i < buffer.size(); ++i) {
        if (std::abs(buffer.data()[i]) > amplitude + 1e-6) {
            withinAmplitude = false;
            break;
        }
    }

    check(
        withinAmplitude,
        "Samples remain within amplitude limits"
    );

    // Test 5: Verify RMS.
    double sumOfSquares{0.0};

    for (std::size_t i{0}; i < buffer.size(); ++i) {
        const double sample = buffer.data()[i];
        sumOfSquares += sample * sample;
    }

    const double rms =
        std::sqrt(sumOfSquares / buffer.size());

    const double expectedRms =
        amplitude / std::sqrt(2.0);

    check(
        approximatelyEqual(rms, expectedRms, 1e-4),
        "RMS matches amplitude / sqrt(2)"
    );

    // Test 6: Estimate frequency using positive-going zero crossings.
    std::size_t crossings{0};

    for (std::size_t i{1}; i < buffer.size(); ++i) {
        const float previous = buffer.data()[i - 1];
        const float current = buffer.data()[i];

        if (previous < 0.0f && current >= 0.0f) {
            ++crossings;
        }
    }

    const double duration =
        static_cast<double>(buffer.size()) / sampleRate;

    const double estimatedFrequency =
        static_cast<double>(crossings) / duration;

    std::cout << "Estimated frequency: "
              << estimatedFrequency << " Hz\n";

    check(
        std::abs(estimatedFrequency - frequency) <= 1.0,
        "Frequency is approximately 440 Hz"
    );

    // Test 7: Zero amplitude produces silence.
    AudioBuffer silentBuffer{100};

    generateSineWave(
        silentBuffer,
        frequency,
        0.0,
        sampleRate
    );

    bool allZero = true;

    for (std::size_t i{0}; i < silentBuffer.size(); ++i) {
        if (silentBuffer.data()[i] != 0.0f) {
            allZero = false;
            break;
        }
    }

    check(
        allZero,
        "Zero amplitude produces silence"
    );

    // Test 8: Invalid sample rate leaves samples unchanged.
    AudioBuffer invalidBuffer{10};

    for (std::size_t i{0}; i < invalidBuffer.size(); ++i) {
        invalidBuffer.data()[i] = 0.25f;
    }

    generateSineWave(
        invalidBuffer,
        frequency,
        amplitude,
        0
    );

    bool unchanged = true;

    for (std::size_t i{0}; i < invalidBuffer.size(); ++i) {
        if (invalidBuffer.data()[i] != 0.25f) {
            unchanged = false;
            break;
        }
    }

    check(
        unchanged,
        "Invalid sample rate leaves buffer unchanged"
    );

    // Test 9: Reject frequencies at or above Nyquist.
    AudioBuffer nyquistBuffer{10};

    for (std::size_t i{0}; i < nyquistBuffer.size(); ++i) {
        nyquistBuffer.data()[i] = 0.25f;
    }

    generateSineWave(
        nyquistBuffer,
        24000.0,
        amplitude,
        sampleRate
    );

    bool nyquistUnchanged = true;

    for (std::size_t i{0}; i < nyquistBuffer.size(); ++i) {
        if (nyquistBuffer.data()[i] != 0.25f) {
            nyquistUnchanged = false;
            break;
        }
    }

    check(
        nyquistUnchanged,
        "Reject frequency at Nyquist limit"
    );

    // Test summary.
    std::cout << "\nTests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

