
#include "tts/audio/decibels.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>

namespace {

int passed = 0;
int failed = 0;

constexpr double kTolerance = 1e-6;

void check(bool condition, const char* description) {
    if (condition) {
        ++passed;
        std::cout << "[PASS] " << description << '\n';
    } else {
        ++failed;
        std::cerr << "[FAIL] " << description << '\n';
    }
}

bool approximatelyEqual(double actual, double expected,
                        double tolerance = kTolerance) {
    return std::isfinite(actual)
        && std::isfinite(expected)
        && std::abs(actual - expected) <= tolerance;
}

void testAmplitudeToDbFS() {
    std::cout << "\n=== Amplitude to dBFS ===\n";

    check(
        approximatelyEqual(amplitudeToDbFS(1.0), 0.0),
        "1.0 amplitude -> 0 dBFS"
    );

    check(
        approximatelyEqual(amplitudeToDbFS(0.5), -6.020599913),
        "0.5 amplitude -> approximately -6.02 dBFS"
    );

    check(
        approximatelyEqual(amplitudeToDbFS(0.1), -20.0),
        "0.1 amplitude -> -20 dBFS"
    );

    check(
        approximatelyEqual(amplitudeToDbFS(0.01), -40.0),
        "0.01 amplitude -> -40 dBFS"
    );

    check(
        approximatelyEqual(amplitudeToDbFS(0.001), -60.0),
        "0.001 amplitude -> -60 dBFS"
    );

    check(
        approximatelyEqual(amplitudeToDbFS(2.0), 6.020599913),
        "2.0 amplitude -> approximately +6.02 dBFS"
    );
}

void testDbFSToAmplitude() {
    std::cout << "\n=== dBFS to Amplitude ===\n";

    check(
        approximatelyEqual(dbFSToAmplitude(0.0), 1.0),
        "0 dBFS -> 1.0 amplitude"
    );

    check(
        approximatelyEqual(dbFSToAmplitude(-6.020599913), 0.5),
        "-6.02 dBFS -> approximately 0.5 amplitude"
    );

    check(
        approximatelyEqual(dbFSToAmplitude(-20.0), 0.1),
        "-20 dBFS -> 0.1 amplitude"
    );

    check(
        approximatelyEqual(dbFSToAmplitude(-40.0), 0.01),
        "-40 dBFS -> 0.01 amplitude"
    );

    check(
        approximatelyEqual(dbFSToAmplitude(-60.0), 0.001),
        "-60 dBFS -> 0.001 amplitude"
    );

    check(
        approximatelyEqual(dbFSToAmplitude(6.020599913), 2.0),
        "+6.02 dBFS -> approximately 2.0 amplitude"
    );
}

void testZeroAndSilence() {
    std::cout << "\n=== Digital Silence ===\n";

    const double negativeInfinity =
        -std::numeric_limits<double>::infinity();

    const double db = amplitudeToDbFS(0.0);

    check(
        std::isinf(db) && db < 0.0,
        "Zero amplitude produces negative infinity dBFS"
    );

    check(
        std::isinf(amplitudeToDbFS(-0.0))
            && amplitudeToDbFS(-0.0) < 0.0,
        "Negative zero represents digital silence"
    );

    check(
        dbFSToAmplitude(negativeInfinity) == 0.0,
        "Negative infinity dBFS produces zero amplitude"
    );
}

void testInvalidAmplitudeInputs() {
    std::cout << "\n=== Invalid Amplitude Inputs ===\n";

    const double nan =
        std::numeric_limits<double>::quiet_NaN();

    const double positiveInfinity =
        std::numeric_limits<double>::infinity();

    const double negativeInfinity = -positiveInfinity;

    check(
        std::isnan(amplitudeToDbFS(-0.5)),
        "Negative amplitude is rejected"
    );

    check(
        std::isnan(amplitudeToDbFS(nan)),
        "NaN amplitude is rejected"
    );

    check(
        std::isnan(amplitudeToDbFS(positiveInfinity)),
        "Positive infinity amplitude is rejected"
    );

    check(
        std::isnan(amplitudeToDbFS(negativeInfinity)),
        "Negative infinity amplitude is rejected"
    );
}

void testInvalidDecibelInputs() {
    std::cout << "\n=== Invalid Decibel Inputs ===\n";

    const double nan =
        std::numeric_limits<double>::quiet_NaN();

    const double positiveInfinity =
        std::numeric_limits<double>::infinity();

    check(
        std::isnan(dbFSToAmplitude(nan)),
        "NaN dBFS is rejected"
    );

    check(
        std::isnan(dbFSToAmplitude(positiveInfinity)),
        "Positive infinity dBFS is rejected"
    );

    check(
        std::isnan(dbFSToAmplitude(10000.0)),
        "Finite dBFS input causing overflow is rejected"
    );
}

void testRoundTripConversions() {
    std::cout << "\n=== Round-Trip Conversions ===\n";

    const double amplitudes[] = {
        1.0,
        0.75,
        0.5,
        0.25,
        0.1,
        0.01,
        0.001,
        2.0
    };

    for (std::size_t i = 0;
         i < sizeof(amplitudes) / sizeof(amplitudes[0]);
         ++i) {

        const double original = amplitudes[i];
        const double db = amplitudeToDbFS(original);
        const double recovered = dbFSToAmplitude(db);

        check(
            approximatelyEqual(recovered, original),
            "Amplitude -> dBFS -> amplitude round trip"
        );
    }

    const double decibelValues[] = {
        0.0,
        -3.0,
        -6.0,
        -12.0,
        -20.0,
        -40.0,
        -60.0,
        6.0
    };

    for (std::size_t i = 0;
         i < sizeof(decibelValues) / sizeof(decibelValues[0]);
         ++i) {

        const double original = decibelValues[i];
        const double amplitude = dbFSToAmplitude(original);
        const double recovered = amplitudeToDbFS(amplitude);

        check(
            approximatelyEqual(recovered, original),
            "dBFS -> amplitude -> dBFS round trip"
        );
    }
}

} // namespace

int main() {
    testAmplitudeToDbFS();
    testDbFSToAmplitude();
    testZeroAndSilence();
    testInvalidAmplitudeInputs();
    testInvalidDecibelInputs();
    testRoundTripConversions();

    std::cout << "\n============================\n";
    std::cout << "Passed: " << passed << '\n';
    std::cout << "Failed: " << failed << '\n';
    std::cout << "============================\n";

    return failed == 0 ? 0 : 1;
}

