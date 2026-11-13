#include "tts/audio/audio_buffer.hpp"
#include "tts/audio/gain.hpp"

#include <cmath>
#include <iostream>
#include <limits>

namespace {

int passed{0};
int failed{0};

void check(bool condition, const char* name) {
    if (condition) {
        ++passed;
        std::cout << "[PASS] " << name << '\n';
    } else {
        ++failed;
        std::cerr << "[FAIL] " << name << '\n';
    }
}

bool near(float actual, float expected) {
    return std::isfinite(actual) &&
           std::abs(actual - expected) <= 1e-6f;
}

void expectSuccess(const char* name, double gainDb,
                   float expectedFirst, float expectedSecond) {
    AudioBuffer buffer{2};
    buffer.data()[0] = 0.6f;
    buffer.data()[1] = -0.4f;

    check(applyGainDb(buffer, gainDb) &&
          buffer.size() == 2 &&
          near(buffer.data()[0], expectedFirst) &&
          near(buffer.data()[1], expectedSecond),
          name);
}

void expectUnchangedOnFailure(const char* name, double gainDb,
                              float first, float second) {
    AudioBuffer buffer{2};
    buffer.data()[0] = first;
    buffer.data()[1] = second;

    check(!applyGainDb(buffer, gainDb) &&
          buffer.size() == 2 &&
          buffer.data()[0] == first &&
          buffer.data()[1] == second,
          name);
}

} // namespace

int main() {
    constexpr double halfGainDb{-6.0206};
    constexpr double doubleGainDb{6.0206};
    constexpr double infinity = std::numeric_limits<double>::infinity();
    constexpr double nan = std::numeric_limits<double>::quiet_NaN();
    constexpr float maxSample = std::numeric_limits<float>::max();

    expectSuccess("0 dB preserves samples", 0.0, 0.6f, -0.4f);
    expectSuccess("Attenuation halves both samples",
                  halfGainDb, 0.3f, -0.2f);
    expectSuccess("Boost preserves values above full scale",
                  doubleGainDb, 1.2f, -0.8f);
    expectSuccess("Negative infinity mutes the buffer",
                  -infinity, 0.0f, 0.0f);

    expectUnchangedOnFailure("Reject NaN gain",
                             nan, 0.6f, -0.4f);
    expectUnchangedOnFailure("Reject positive infinity gain",
                             infinity, 0.6f, -0.4f);
    expectUnchangedOnFailure("Reject a gain whose multiplier overflows",
                             10000.0, 0.6f, -0.4f);
    expectUnchangedOnFailure("Positive sample overflow preserves both samples",
                             doubleGainDb, 0.5f, maxSample);
    expectUnchangedOnFailure("Negative sample overflow preserves both samples",
                             doubleGainDb, 0.5f, -maxSample);

    std::cout << "\nPassed: " << passed << "\nFailed: " << failed << '\n';
    return failed == 0 ? 0 : 1;
}
