// tests/test_analyzer.cpp
#include <cmath>
#include <iostream>
#include "tts/audio/analyzer.hpp"

using AnalyzerFunction = bool (*)(const float*, int, float*);

int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, const char* testName) {
    if (condition) {
        std::cout << "PASS: " << testName << '\n';
        ++testsPassed;
    } else {
        std::cerr << "FAIL: " << testName << '\n';
        ++testsFailed;
    }
}

bool approximatelyEqual(float actual, float expected) {
    return std::fabs(actual - expected) < 0.00001f;
}

void testValidInput(
    AnalyzerFunction function,
    const float* samples,
    int count,
    float expected,
    const char* testName
) {
    float result = 123.0f;

    bool success = function(samples, count, &result);

    check(
        success && approximatelyEqual(result, expected),
        testName
    );
}

void testInvalidInput(
    AnalyzerFunction function,
    const float* samples,
    int count,
    bool nullResult,
    const char* testName
) {
    float result = 123.0f;
    float* resultPointer = nullResult ? nullptr : &result;

    bool success = function(samples, count, resultPointer);

    check(
        !success && result == 123.0f,
        testName
    );
}

void runAnalyzerTests(
    AnalyzerFunction function,
    const char* functionName,
    float normalExpected,
    float negativeExpected,
    float positiveExpected
) {
    const float normal[] = {0.2f, -0.5f, 0.8f, -0.3f, 0.1f};
    const float negative[] = {-0.8f, -0.5f, -0.2f};
    const float positive[] = {0.3f, 0.7f, 0.1f};
    const float single[] = {0.5f};

    std::cout << "\nTesting " << functionName << '\n';

    testValidInput(
        function, normal, 5, normalExpected,
        "Normal samples"
    );

    testValidInput(
        function, negative, 3, negativeExpected,
        "All negative samples"
    );

    testValidInput(
        function, positive, 3, positiveExpected,
        "All positive samples"
    );

    testValidInput(
        function, single, 1, 0.5f,
        "Single sample"
    );

    testInvalidInput(
        function, nullptr, 5, false,
        "Null sample pointer"
    );

    testInvalidInput(
        function, normal, 5, true,
        "Null result pointer"
    );

    testInvalidInput(
        function, normal, 0, false,
        "Zero sample count"
    );

    testInvalidInput(
        function, normal, -1, false,
        "Negative sample count"
    );
}

int main() {
    runAnalyzerTests(
        findMaxSample,
        "findMaxSample",
        0.8f,
        -0.2f,
        0.7f
    );

    runAnalyzerTests(
        findMinSample,
        "findMinSample",
        -0.5f,
        -0.8f,
        0.1f
    );

    runAnalyzerTests(
        calculateMeanAmplitude,
        "calculateMeanAmplitude",
        0.38f,
        0.5f,
        1.1f / 3.0f
    );

    std::cout << "\n--------------------\n";
    std::cout << "Passed: " << testsPassed << '\n';
    std::cout << "Failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

