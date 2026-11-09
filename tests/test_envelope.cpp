
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/envelope.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>

namespace {

int testsPassed{0};
int testsFailed{0};

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
    return std::isfinite(actual) &&
           std::abs(actual - expected) <= tolerance;
}

void fillBuffer(AudioBuffer& buffer, float value) {
    for (std::size_t i{0}; i < buffer.size(); ++i) {
        buffer.data()[i] = value;
    }
}

bool allSamplesEqual(const AudioBuffer& buffer, float expected) {
    // AudioBuffer currently exposes data() as a non-const member.
    // Use a non-const reference in this helper until a const
    // data() overload is added.
    return false;
}

bool allSamplesEqual(AudioBuffer& buffer, float expected) {
    for (std::size_t i{0}; i < buffer.size(); ++i) {
        if (!approximatelyEqual(buffer.data()[i], expected)) {
            return false;
        }
    }
    return true;
}

} // namespace

int main() {
    constexpr std::uint32_t sampleRate{1000};

    // 1000 samples at 1000 Hz = 1 second.
    // Attack = 200 samples.
    // Sustain = 600 samples.
    // Release = 200 samples.
    AudioBuffer buffer{1000};
    fillBuffer(buffer, 1.0f);

    // Test 1: Apply a standard envelope.
    check(
        applyEnvelope(buffer, 0.2, 0.2, sampleRate),
        "Apply standard envelope"
    );

    check(
        buffer.size() == 1000,
        "Envelope preserves buffer size"
    );

    // Attack: indices 0 through 199.
    check(
        approximatelyEqual(buffer.data()[0], 0.0),
        "Attack starts at zero"
    );

    check(
        approximatelyEqual(buffer.data()[199], 1.0),
        "Attack ends at full gain"
    );

    check(
        approximatelyEqual(buffer.data()[100], 100.0 / 199.0),
        "Attack midpoint follows linear ramp"
    );

    // Sustain: indices 200 through 799.
    check(
        approximatelyEqual(buffer.data()[200], 1.0),
        "Sustain begins at full gain"
    );

    check(
        approximatelyEqual(buffer.data()[500], 1.0),
        "Sustain maintains full gain"
    );

    check(
        approximatelyEqual(buffer.data()[799], 1.0),
        "Sustain ends at full gain"
    );

    // Release: indices 800 through 999.
    check(
        approximatelyEqual(buffer.data()[800], 1.0),
        "Release starts at full gain"
    );

    check(
        approximatelyEqual(buffer.data()[900], 1.0 - 100.0 / 199.0),
        "Release midpoint follows linear ramp"
    );

    check(
        approximatelyEqual(buffer.data()[999], 0.0),
        "Release ends at zero"
    );

    // Test 2: Zero-duration envelope.
    AudioBuffer unchangedBuffer{100};
    fillBuffer(unchangedBuffer, 0.75f);

    check(
        applyEnvelope(unchangedBuffer, 0.0, 0.0, sampleRate),
        "Accept zero attack and release"
    );

    check(
        allSamplesEqual(unchangedBuffer, 0.75f),
        "Zero-duration envelope preserves samples"
    );

    // Test 3: Single-sample attack.
    AudioBuffer shortAttack{10};
    std::cout << "One-sample attack value: " 
            << shortAttack.data()[0] << '\n';

    fillBuffer(shortAttack, 1.0f);

    check(
        applyEnvelope(shortAttack, 0.001, 0.0, sampleRate),
        "Accept one-sample attack"
    );

    check(
        approximatelyEqual(shortAttack.data()[0], 1.0),
        "One-sample attack uses full gain"
    );

    // Test 4: Single-sample release.
    AudioBuffer shortRelease{10};
    fillBuffer(shortRelease, 1.0f);

    check(
        applyEnvelope(shortRelease, 0.0, 0.001, sampleRate),
        "Accept one-sample release"
    );

    check(
        approximatelyEqual(shortRelease.data()[9], 0.0),
        "One-sample release ends at zero"
    );

    check(
        approximatelyEqual(shortRelease.data()[8], 1.0),
        "Samples before release remain unchanged"
    );

    // Test 5: No sustain stage.
    AudioBuffer noSustain{1000};
    fillBuffer(noSustain, 1.0f);

    check(
        applyEnvelope(noSustain, 0.5, 0.5, sampleRate),
        "Accept attack and release without sustain"
    );

    check(
        approximatelyEqual(noSustain.data()[0], 0.0),
        "No-sustain attack starts at zero"
    );

    check(
        approximatelyEqual(noSustain.data()[499], 1.0),
        "No-sustain attack reaches full gain"
    );

    check(
        approximatelyEqual(noSustain.data()[500], 1.0),
        "No-sustain release starts at full gain"
    );

    check(
        approximatelyEqual(noSustain.data()[999], 0.0),
        "No-sustain release ends at zero"
    );

    // Test 6: Invalid sample rate.
    AudioBuffer invalidRate{100};
    fillBuffer(invalidRate, 0.5f);

    check(
        !applyEnvelope(invalidRate, 0.2, 0.2, 0),
        "Reject zero sample rate"
    );

    check(
        allSamplesEqual(invalidRate, 0.5f),
        "Invalid sample rate preserves buffer"
    );

    // Test 7: Negative durations.
    AudioBuffer negativeDuration{100};
    fillBuffer(negativeDuration, 0.5f);

    check(
        !applyEnvelope(negativeDuration, -0.1, 0.0, sampleRate),
        "Reject negative attack duration"
    );

    check(
        !applyEnvelope(negativeDuration, 0.0, -0.1, sampleRate),
        "Reject negative release duration"
    );

    check(
        allSamplesEqual(negativeDuration, 0.5f),
        "Negative durations preserve buffer"
    );

    // Test 8: Non-finite durations.
    const double nan =
        std::numeric_limits<double>::quiet_NaN();

    const double infinity =
        std::numeric_limits<double>::infinity();

    AudioBuffer nonFinite{100};
    fillBuffer(nonFinite, 0.5f);

    check(
        !applyEnvelope(nonFinite, nan, 0.0, sampleRate),
        "Reject NaN attack"
    );

    check(
        !applyEnvelope(nonFinite, 0.0, nan, sampleRate),
        "Reject NaN release"
    );

    check(
        !applyEnvelope(nonFinite, infinity, 0.0, sampleRate),
        "Reject infinite attack"
    );

    check(
        !applyEnvelope(nonFinite, 0.0, infinity, sampleRate),
        "Reject infinite release"
    );

    check(
        allSamplesEqual(nonFinite, 0.5f),
        "Non-finite durations preserve buffer"
    );

    // Test 9: Durations exceeding buffer length.
    AudioBuffer oversized{1000};
    fillBuffer(oversized, 0.5f);

    check(
        !applyEnvelope(oversized, 0.7, 0.5, sampleRate),
        "Reject attack and release exceeding duration"
    );

    check(
        allSamplesEqual(oversized, 0.5f),
        "Oversized envelope preserves buffer"
    );

    // Test 10: Empty buffer.
    AudioBuffer empty{0};

    check(
        applyEnvelope(empty, 0.0, 0.0, sampleRate),
        "Accept empty buffer with zero-duration envelope"
    );

    // Summary.
    std::cout << "\nTests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

