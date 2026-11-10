
#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/mixer.hpp"

#include <cmath>
#include <cstddef>
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
    double tolerance = 1e-6
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

} // namespace

int main() {
    // -------------------------------------------------
    // Test 1: Basic mixing
    // -------------------------------------------------

    AudioBuffer inputA{4};
    AudioBuffer inputB{4};
    AudioBuffer output{4};

    inputA.data()[0] = 0.2f;
    inputA.data()[1] = 0.4f;
    inputA.data()[2] = -0.2f;
    inputA.data()[3] = 0.5f;

    inputB.data()[0] = 0.3f;
    inputB.data()[1] = 0.1f;
    inputB.data()[2] = 0.4f;
    inputB.data()[3] = -0.5f;

    check(
        mixAudio(output, inputA, inputB, 1.0f, 1.0f),
        "Basic mixing succeeds"
    );

    check(
        approximatelyEqual(output.data()[0], 0.5),
        "First mixed sample"
    );

    check(
        approximatelyEqual(output.data()[1], 0.5),
        "Second mixed sample"
    );

    check(
        approximatelyEqual(output.data()[2], 0.2),
        "Third mixed sample"
    );

    check(
        approximatelyEqual(output.data()[3], 0.0),
        "Fourth mixed sample"
    );

    // -------------------------------------------------
    // Test 2: Independent gain control
    // -------------------------------------------------

    check(
        mixAudio(output, inputA, inputB, 0.5f, 0.25f),
        "Independent gain mixing succeeds"
    );

    check(
        approximatelyEqual(output.data()[0], 0.175),
        "Independent gains: sample 0"
    );

    check(
        approximatelyEqual(output.data()[1], 0.225),
        "Independent gains: sample 1"
    );

    check(
        approximatelyEqual(output.data()[2], 0.0),
        "Independent gains: sample 2"
    );

    check(
        approximatelyEqual(output.data()[3], 0.125),
        "Independent gains: sample 3"
    );

    // -------------------------------------------------
    // Test 3: Muting inputs
    // -------------------------------------------------

    check(
        mixAudio(output, inputA, inputB, 1.0f, 0.0f),
        "Mute input B"
    );

    check(
        approximatelyEqual(output.data()[0], 0.2),
        "Muted B preserves input A"
    );

    check(
        mixAudio(output, inputA, inputB, 0.0f, 1.0f),
        "Mute input A"
    );

    check(
        approximatelyEqual(output.data()[0], 0.3),
        "Muted A preserves input B"
    );

    check(
        mixAudio(output, inputA, inputB, 0.0f, 0.0f),
        "Mute both inputs"
    );

    check(
        allSamplesEqual(output, 0.0f),
        "Muting both inputs produces silence"
    );

    // -------------------------------------------------
    // Test 4: Floating-point headroom
    // -------------------------------------------------

    AudioBuffer loudA{2};
    AudioBuffer loudB{2};
    AudioBuffer loudOutput{2};

    fillBuffer(loudA, 0.8f);
    fillBuffer(loudB, 0.7f);

    check(
        mixAudio(loudOutput, loudA, loudB, 1.0f, 1.0f),
        "Mix signals exceeding normalized range"
    );

    check(
        allSamplesEqual(loudOutput, 1.5f),
        "Mixer preserves values above 1.0"
    );

    check(
        mixAudio(loudOutput, loudA, loudB, 0.5f, 0.5f),
        "Reduce gains to create headroom"
    );

    check(
        allSamplesEqual(loudOutput, 0.75f),
        "Reduced gains prevent clipping"
    );

    // -------------------------------------------------
    // Test 5: Input buffers remain unchanged
    // -------------------------------------------------

    check(
        approximatelyEqual(inputA.data()[0], 0.2),
        "Input A remains unchanged"
    );

    check(
        approximatelyEqual(inputB.data()[0], 0.3),
        "Input B remains unchanged"
    );

    // -------------------------------------------------
    // Test 6: Mismatched buffer sizes
    // -------------------------------------------------

    AudioBuffer shortBuffer{2};

    fillBuffer(output, 0.25f);

    check(
        !mixAudio(output, inputA, shortBuffer, 1.0f, 1.0f),
        "Reject mismatched input sizes"
    );

    check(
        allSamplesEqual(output, 0.25f),
        "Size mismatch preserves output"
    );

    AudioBuffer shortOutput{2};
    fillBuffer(shortOutput, 0.25f);

    check(
        !mixAudio(shortOutput, inputA, inputB, 1.0f, 1.0f),
        "Reject mismatched output size"
    );

    check(
        allSamplesEqual(shortOutput, 0.25f),
        "Output size mismatch preserves output"
    );

    // -------------------------------------------------
    // Test 7: Invalid gains
    // -------------------------------------------------

    const float nan =
        std::numeric_limits<float>::quiet_NaN();

    const float infinity =
        std::numeric_limits<float>::infinity();

    fillBuffer(output, 0.25f);

    check(
        !mixAudio(output, inputA, inputB, nan, 1.0f),
        "Reject NaN gain A"
    );

    check(
        !mixAudio(output, inputA, inputB, 1.0f, nan),
        "Reject NaN gain B"
    );

    check(
        !mixAudio(output, inputA, inputB, infinity, 1.0f),
        "Reject infinite gain A"
    );

    check(
        !mixAudio(output, inputA, inputB, 1.0f, infinity),
        "Reject infinite gain B"
    );

    check(
        allSamplesEqual(output, 0.25f),
        "Invalid gains preserve output"
    );

    // -------------------------------------------------
    // Test 8: Empty buffers
    // -------------------------------------------------

    AudioBuffer emptyA{0};
    AudioBuffer emptyB{0};
    AudioBuffer emptyOutput{0};

    check(
        mixAudio(emptyOutput, emptyA, emptyB, 1.0f, 1.0f),
        "Accept three empty buffers"
    );

    check(
        emptyOutput.size() == 0,
        "Empty output remains empty"
    );

    // -------------------------------------------------
    // Test 9: Negative samples and phase cancellation
    // -------------------------------------------------

    AudioBuffer positive{3};
    AudioBuffer negative{3};
    AudioBuffer cancellation{3};

    fillBuffer(positive, 0.5f);
    fillBuffer(negative, -0.5f);

    check(
        mixAudio(cancellation, positive, negative, 1.0f, 1.0f),
        "Mix opposite-polarity signals"
    );

    check(
        allSamplesEqual(cancellation, 0.0f),
        "Opposite-polarity signals cancel"
    );

    // -------------------------------------------------
    // Test summary
    // -------------------------------------------------

    std::cout << "\nTests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

