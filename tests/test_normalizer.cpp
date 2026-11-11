#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/normalizer.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <algorithm>
#include <initializer_list>

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

bool allSamplesEqual(
    const AudioBuffer& buffer,
    float expected
) {
    for (std::size_t i{0}; i < buffer.size(); ++i) {
        if (!approximatelyEqual(buffer.data()[i], expected)) {
            return false;
        }
    }

    return true;
}

bool matchesSamples(
    const AudioBuffer& buffer,
    std::initializer_list<float> expected
) {
    if (buffer.size() != expected.size()) {
        return false;
    }

    std::size_t i{0};

    for (float value : expected) {
        if (!approximatelyEqual(buffer.data()[i], value)) {
            return false;
        }
        ++i;
    }

    return true;
}

} // namespace

int main() {
    // -------------------------------------------------
    // Test 1: Amplify a quiet signal
    // -------------------------------------------------

    AudioBuffer quiet{3};

    quiet.data()[0] = 0.2f;
    quiet.data()[1] = -0.4f;
    quiet.data()[2] = 0.1f;

    check(
        normalizePeak(quiet, 0.8f),
        "Amplify quiet signal"
    );

    check(
        matchesSamples(quiet, {0.4f, -0.8f, 0.2f}),
        "Amplified samples match expected values"
    );

    // -------------------------------------------------
    // Test 2: Attenuate a loud signal
    // -------------------------------------------------

    AudioBuffer loud{4};

    loud.data()[0] = 0.5f;
    loud.data()[1] = -1.4f;
    loud.data()[2] = 0.7f;
    loud.data()[3] = -0.35f;

    check(
        normalizePeak(loud, 0.7f),
        "Attenuate loud signal"
    );

    check(
        matchesSamples(loud, {0.25f, -0.7f, 0.35f, -0.175f}),
        "Attenuated samples match expected values"
    );

    // -------------------------------------------------
    // Test 3: Preserve signal already at target peak
    // -------------------------------------------------

    AudioBuffer atTarget{3};

    atTarget.data()[0] = 0.3f;
    atTarget.data()[1] = -0.8f;
    atTarget.data()[2] = 0.2f;

    check(
        normalizePeak(atTarget, 0.8f),
        "Accept signal already at target peak"
    );

    check(
        matchesSamples(atTarget, {0.3f, -0.8f, 0.2f}),
        "Signal at target peak remains unchanged"
    );

    // -------------------------------------------------
    // Test 4: Silent buffer
    // -------------------------------------------------

    AudioBuffer silent{100};
    fillBuffer(silent, 0.0f);

    check(
        normalizePeak(silent, 0.9f),
        "Accept silent buffer"
    );

    check(
        allSamplesEqual(silent, 0.0f),
        "Silent buffer remains silent"
    );

    // -------------------------------------------------
    // Test 5: Normalize to zero
    // -------------------------------------------------

    AudioBuffer muted{3};

    muted.data()[0] = 0.5f;
    muted.data()[1] = -0.8f;
    muted.data()[2] = 0.3f;

    check(
        normalizePeak(muted, 0.0f),
        "Normalize non-silent buffer to zero"
    );

    check(
        allSamplesEqual(muted, 0.0f),
        "Target peak zero produces silence"
    );

    // -------------------------------------------------
    // Test 6: Reject negative target peaks
    // -------------------------------------------------

    AudioBuffer negativeTarget{3};
    fillBuffer(negativeTarget, 0.5f);

    check(
        !normalizePeak(negativeTarget, -0.5f),
        "Reject negative target peak"
    );

    check(
        allSamplesEqual(negativeTarget, 0.5f),
        "Negative target preserves buffer"
    );

    // -------------------------------------------------
    // Test 7: Reject target peaks above 1.0
    // -------------------------------------------------

    AudioBuffer oversizedTarget{3};
    fillBuffer(oversizedTarget, 0.5f);

    check(
        !normalizePeak(oversizedTarget, 1.1f),
        "Reject target above normalized range"
    );

    check(
        allSamplesEqual(oversizedTarget, 0.5f),
        "Oversized target preserves buffer"
    );

    // -------------------------------------------------
    // Test 8: Reject NaN target peak
    // -------------------------------------------------

    const float nan =
        std::numeric_limits<float>::quiet_NaN();

    AudioBuffer nanTarget{3};
    fillBuffer(nanTarget, 0.5f);

    check(
        !normalizePeak(nanTarget, nan),
        "Reject NaN target peak"
    );

    check(
        allSamplesEqual(nanTarget, 0.5f),
        "NaN target preserves buffer"
    );

    // -------------------------------------------------
    // Test 9: Reject infinite target peak
    // -------------------------------------------------

    const float infinity =
        std::numeric_limits<float>::infinity();

    AudioBuffer infinityTarget{3};
    fillBuffer(infinityTarget, 0.5f);

    check(
        !normalizePeak(infinityTarget, infinity),
        "Reject infinite target peak"
    );

    check(
        allSamplesEqual(infinityTarget, 0.5f),
        "Infinite target preserves buffer"
    );

    // -------------------------------------------------
    // Test 10: Reject NaN audio samples
    // -------------------------------------------------

    AudioBuffer nanSamples{3};

    nanSamples.data()[0] = 0.2f;
    nanSamples.data()[1] = nan;
    nanSamples.data()[2] = 0.4f;

    check(
        !normalizePeak(nanSamples, 0.8f),
        "Reject NaN audio samples"
    );

    check(
        approximatelyEqual(nanSamples.data()[0], 0.2f) &&
        std::isnan(nanSamples.data()[1]) &&
        approximatelyEqual(nanSamples.data()[2], 0.4f),
        "NaN sample failure preserves buffer"
    );

    // -------------------------------------------------
    // Test 11: Reject infinite audio samples
    // -------------------------------------------------

    AudioBuffer infiniteSamples{3};

    infiniteSamples.data()[0] = 0.2f;
    infiniteSamples.data()[1] = infinity;
    infiniteSamples.data()[2] = 0.4f;

    check(
        !normalizePeak(infiniteSamples, 0.8f),
        "Reject infinite audio samples"
    );

    check(
        approximatelyEqual(infiniteSamples.data()[0], 0.2f) &&
        std::isinf(infiniteSamples.data()[1]) &&
        approximatelyEqual(infiniteSamples.data()[2], 0.4f),
        "Infinite sample failure preserves buffer"
    );

    // -------------------------------------------------
    // Test 12: Empty buffer
    // -------------------------------------------------

    AudioBuffer empty{0};

    check(
        normalizePeak(empty, 0.9f),
        "Accept empty buffer"
    );

    check(
        empty.size() == 0,
        "Empty buffer remains empty"
    );

    // -------------------------------------------------
    // Test 13: Peak detection uses absolute amplitude
    // -------------------------------------------------

    AudioBuffer negativePeak{3};

    negativePeak.data()[0] = 0.2f;
    negativePeak.data()[1] = -0.9f;
    negativePeak.data()[2] = 0.4f;

    check(
        normalizePeak(negativePeak, 0.9f),
        "Normalize signal with negative peak"
    );

    check(
        matchesSamples(negativePeak, {0.2f, -0.9f, 0.4f}),
        "Negative magnitude determines peak"
    );

    // -------------------------------------------------
    // Test 14: Verify final peak after normalization
    // -------------------------------------------------

    AudioBuffer peakBuffer{5};

    peakBuffer.data()[0] = 0.4f;
    peakBuffer.data()[1] = -0.8f;
    peakBuffer.data()[2] = 1.4f;
    peakBuffer.data()[3] = -1.2f;
    peakBuffer.data()[4] = 0.6f;

    check(
        normalizePeak(peakBuffer, 0.9f),
        "Normalize signal with peak above 1.0"
    );

    float measuredPeak{0.0f};

    for (std::size_t i{0}; i < peakBuffer.size(); ++i) {
        measuredPeak = std::max(
            measuredPeak,
            std::abs(peakBuffer.data()[i])
        );
    }

    check(
        approximatelyEqual(measuredPeak, 0.9),
        "Final peak matches target amplitude"
    );

    // -------------------------------------------------
    // Test 15: Buffer size is preserved
    // -------------------------------------------------

    check(
        peakBuffer.size() == 5,
        "Normalization preserves sample count"
    );

    // -------------------------------------------------
    // Summary
    // -------------------------------------------------

    std::cout << "\nTests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

