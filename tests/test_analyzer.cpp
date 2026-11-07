
#include "tts/audio/analyzer.hpp"

#include <cmath>
#include <iostream>

// Function pointer type for analyzer functions
using AnalyzerFunction = bool (*)(const float*, int, float*);

// Compare floating-point values within a tolerance
bool approxEqual(float actual, float expected) {
    constexpr float tolerance = 0.00001f;
    return std::fabs(actual - expected) < tolerance;
}

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

// Test one analyzer function against eight cases
void testAnalyzer(
    AnalyzerFunction function,
    const char* functionName,
    float expectedMixed,
    float expectedNegative,
    float expectedPositive,
    float expectedSingle
) {
    float result{};

    // Test 1: Mixed positive and negative values
    const float mixed[] = {0.2f, -0.5f, 0.8f, -0.3f, 0.1f};

    result = -99.0f;
    bool success = function(mixed, 5, &result);

    check(success && approxEqual(result, expectedMixed),
          functionName);

    // Test 2: All negative samples
    const float negative[] = {-0.2f, -0.5f, -0.8f};

    result = -99.0f;
    success = function(negative, 3, &result);

    check(success && approxEqual(result, expectedNegative),
          "All negative samples");

    // Test 3: All positive samples
    const float positive[] = {0.2f, 0.5f, 0.8f};

    result = -99.0f;
    success = function(positive, 3, &result);

    check(success && approxEqual(result, expectedPositive),
          "All positive samples");

    // Test 4: Single sample
    const float single[] = {-0.5f};

    result = -99.0f;
    success = function(single, 1, &result);

    check(success && approxEqual(result, expectedSingle),
          "Single sample");

    // Test 5: Null samples pointer
    result = -99.0f;
    success = function(nullptr, 3, &result);

    check(!success && result == -99.0f,
          "Null samples pointer");

    // Test 6: Null result pointer
    success = function(mixed, 5, nullptr);

    check(!success,
          "Null result pointer");

    // Test 7: Zero sample count
    result = -99.0f;
    success = function(mixed, 0, &result);

    check(!success && result == -99.0f,
          "Zero sample count");

    // Test 8: Negative sample count
    result = -99.0f;
    success = function(mixed, -1, &result);

    check(!success && result == -99.0f,
          "Negative sample count");
}

int main() {
    std::cout << "=== Audio Analyzer Tests ===\n\n";

    // ----------------------------------------
    // Maximum Sample Tests
    // ----------------------------------------

    std::cout << "--- findMaxSample ---\n";

    testAnalyzer(
        findMaxSample,
        "Mixed samples - Max",
        0.8f,
        -0.2f,
        0.8f,
        -0.5f
    );

    // ----------------------------------------
    // Minimum Sample Tests
    // ----------------------------------------

    std::cout << "\n--- findMinSample ---\n";

    testAnalyzer(
        findMinSample,
        "Mixed samples - Min",
        -0.5f,
        -0.8f,
        0.2f,
        -0.5f
    );

    // ----------------------------------------
    // Mean Absolute Amplitude Tests
    // ----------------------------------------

    std::cout << "\n--- calculateMeanAmplitude ---\n";

    testAnalyzer(
        calculateMeanAmplitude,
        "Mixed samples - Mean",
        0.38f,
        0.5f,
        0.5f,
        0.5f
    );

    // ----------------------------------------
    // RMS Amplitude Tests
    // ----------------------------------------

    std::cout << "\n--- calculateRMS ---\n";

    // RMS of mixed samples:
    // sqrt((0.04 + 0.25 + 0.64 + 0.09 + 0.01) / 5)
    // = sqrt(0.206)
    testAnalyzer(
        calculateRMS,
        "Mixed samples - RMS",
        std::sqrt(0.206f),
        std::sqrt(0.31f),
        std::sqrt(0.31f),
        0.5f
    );

    // Additional RMS-specific test cases

    float result{};

    // Constant positive samples
    const float constantPositive[] = {
        0.8f, 0.8f, 0.8f, 0.8f
    };

    bool success = calculateRMS(
        constantPositive, 4, &result
    );

    check(success && approxEqual(result, 0.8f),
          "RMS - Constant positive");

    // Mixed silence and signal
    const float mixedSilence[] = {
        0.0f, 0.0f, 0.0f, 0.8f
    };

    result = -99.0f;
    success = calculateRMS(mixedSilence, 4, &result);

    check(success && approxEqual(result, 0.4f),
          "RMS - Mixed silence");

    // Constant negative samples
    const float constantNegative[] = {
        -0.5f, -0.5f
    };

    result = -99.0f;
    success = calculateRMS(constantNegative, 2, &result);

    check(success && approxEqual(result, 0.5f),
          "RMS - Constant negative");

    // All silence
    const float silence[] = {
        0.0f, 0.0f
    };

    result = -99.0f;
    success = calculateRMS(silence, 2, &result);

    check(success && approxEqual(result, 0.0f),
          "RMS - Silence");

    // ----------------------------------------
    // Final Results
    // ----------------------------------------

    std::cout << "\n============================\n";
    std::cout << "Tests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    if (testsFailed != 0) {
        std::cerr << "Audio Analyzer tests failed!\n";
        return 1;
    }

    std::cout << "All Audio Analyzer tests passed!\n";
    return 0;
}

