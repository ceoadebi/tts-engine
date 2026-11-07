
#include "tts/audio/pcm.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

// Track test results
int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, const char* testName) {
    if (condition) {
        std::cout << "[PASS] " << testName << '\n';
        ++testsPassed;
    } else {
        std::cerr << "[FAIL] " << testName << '\n';
        ++testsFailed;
    }
}

int main() {
    std::cout << "=== PCM16 Conversion Tests ===\n\n";

    // ----------------------------------------
    // Test 1: Positive full scale
    // ----------------------------------------

    check(floatToPCM16(1.0f) == 32767,
          "Positive full scale");

    // ----------------------------------------
    // Test 2: Negative full scale
    // ----------------------------------------

    check(floatToPCM16(-1.0f) == -32768,
          "Negative full scale");

    // ----------------------------------------
    // Test 3: Silence
    // ----------------------------------------

    check(floatToPCM16(0.0f) == 0,
          "Silence");

    // ----------------------------------------
    // Test 4: Positive half scale
    // ----------------------------------------

    check(floatToPCM16(0.5f) == 16384,
          "Positive half scale");

    // ----------------------------------------
    // Test 5: Negative half scale
    // ----------------------------------------

    check(floatToPCM16(-0.5f) == -16384,
          "Negative half scale");

    // ----------------------------------------
    // Test 6: Positive quarter scale
    // ----------------------------------------

    check(floatToPCM16(0.25f) == 8192,
          "Positive quarter scale");

    // ----------------------------------------
    // Test 7: Negative quarter scale
    // ----------------------------------------

    check(floatToPCM16(-0.25f) == -8192,
          "Negative quarter scale");

    // ----------------------------------------
    // Test 8: Positive clipping
    // ----------------------------------------

    check(floatToPCM16(1.5f) == 32767,
          "Positive clipping");

    // ----------------------------------------
    // Test 9: Negative clipping
    // ----------------------------------------

    check(floatToPCM16(-1.5f) == -32768,
          "Negative clipping");

    // ----------------------------------------
    // Test 10: Large positive input
    // ----------------------------------------

    check(floatToPCM16(1000.0f) == 32767,
          "Large positive input");

    // ----------------------------------------
    // Test 11: Large negative input
    // ----------------------------------------

    check(floatToPCM16(-1000.0f) == -32768,
          "Large negative input");

    // ----------------------------------------
    // Test 12: NaN
    // ----------------------------------------

    const float nan =
        std::numeric_limits<float>::quiet_NaN();

    check(floatToPCM16(nan) == 0,
          "NaN produces silence");

    // ----------------------------------------
    // Test 13: Positive infinity
    // ----------------------------------------

    const float positiveInfinity =
        std::numeric_limits<float>::infinity();

    check(floatToPCM16(positiveInfinity) == 0,
          "Positive infinity produces silence");

    // ----------------------------------------
    // Test 14: Negative infinity
    // ----------------------------------------

    const float negativeInfinity =
        -std::numeric_limits<float>::infinity();

    check(floatToPCM16(negativeInfinity) == 0,
          "Negative infinity produces silence");

    // ----------------------------------------
    // Test 15: Positive rounding
    // ----------------------------------------

    // 0.5 / 32767 * 32767 = 0.5
    // std::round(0.5) = 1

    const float positiveHalfLSB = 0.5f / 32767.0f;

    check(floatToPCM16(positiveHalfLSB) == 1,
          "Positive half-LSB rounding");

    // ----------------------------------------
    // Test 16: Negative rounding
    // ----------------------------------------

    // -0.5 / 32768 * 32768 = -0.5
    // std::round(-0.5) = -1

    const float negativeHalfLSB = -0.5f / 32768.0f;

    check(floatToPCM16(negativeHalfLSB) == -1,
          "Negative half-LSB rounding");

    // ----------------------------------------
    // Test 17: Small positive sample
    // ----------------------------------------

    check(floatToPCM16(0.000001f) == 0,
          "Small positive sample rounds to zero");

    // ----------------------------------------
    // Test 18: Small negative sample
    // ----------------------------------------

    check(floatToPCM16(-0.000001f) == 0,
          "Small negative sample rounds to zero");

    // ----------------------------------------
    // Final Results
    // ----------------------------------------

    std::cout << "\n============================\n";
    std::cout << "Tests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    if (testsFailed != 0) {
        std::cerr << "PCM16 conversion tests failed!\n";
        return 1;
    }

    std::cout << "All PCM16 conversion tests passed!\n";
    return 0;
}

