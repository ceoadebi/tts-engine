
#include "tts/audio/envelope.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

bool applyEnvelope(
    AudioBuffer& buffer,
    double attackSeconds,
    double releaseSeconds,
    std::uint32_t sampleRate
) {
    if (sampleRate == 0 ||
        !std::isfinite(attackSeconds) ||
        !std::isfinite(releaseSeconds) ||
        attackSeconds < 0.0 ||
        releaseSeconds < 0.0) {
        return false;
    }

    const std::size_t totalSamples = buffer.size();

    const double totalDuration =
        static_cast<double>(totalSamples) / sampleRate;

    if (attackSeconds > totalDuration ||
        releaseSeconds > totalDuration - attackSeconds) {
        return false;
    }

    const double attackCount = attackSeconds * sampleRate;
    const double releaseCount = releaseSeconds * sampleRate;

    if (attackCount > static_cast<double>(totalSamples) ||
        releaseCount > static_cast<double>(totalSamples) - attackCount) {
        return false;
    }

    const auto attackSamples =
        static_cast<std::size_t>(std::round(attackCount));

    const auto releaseSamples =
        static_cast<std::size_t>(std::round(releaseCount));

    if (attackSamples > totalSamples ||
        releaseSamples > totalSamples - attackSamples) {
        return false;
    }

    const std::size_t releaseStart =
        totalSamples - releaseSamples;

    for (std::size_t i{0}; i < totalSamples; ++i) {
        double gain{1.0};

        if (i < attackSamples) {
            if (attackSamples > 1) {
                gain = static_cast<double>(i)
                     / (attackSamples - 1);
            }
        }

        else if (i >= releaseStart) {
            if (releaseSamples == 1) {
                gain = 0.0;
            } else {
                const std::size_t releaseIndex =
                    i - releaseStart;

                gain = 1.0 -
                    static_cast<double>(releaseIndex)
                    / (releaseSamples - 1);
            }
        }

        buffer.data()[i] = static_cast<float>(
            static_cast<double>(buffer.data()[i]) * gain
        );
    }

    return true;
}

