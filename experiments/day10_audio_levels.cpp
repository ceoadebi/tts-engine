
#include "tts/audio/decibels.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

int main() {
    const std::string filename = "mixed_440_660_normalized.wav";

    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open: " << filename << '\n';
        return 1;
    }

    // Read the standard 44-byte PCM WAV header.
    char header[44];

    if (!file.read(header, sizeof(header))) {
        std::cerr << "Failed to read WAV header\n";
        return 1;
    }

    // This experiment expects the mono PCM16 WAV
    // produced by the Day 9 audio pipeline.
    const auto byteAt = [&](std::size_t offset) {
        return static_cast<std::uint8_t>(header[offset]);
    };

    const auto readU16 = [&](std::size_t offset) {
        return static_cast<std::uint16_t>(
            byteAt(offset) |
            (static_cast<std::uint16_t>(byteAt(offset + 1)) << 8)
        );
    };

    const auto readU32 = [&](std::size_t offset) {
        return static_cast<std::uint32_t>(
            static_cast<std::uint32_t>(byteAt(offset)) |
            (static_cast<std::uint32_t>(byteAt(offset + 1)) << 8) |
            (static_cast<std::uint32_t>(byteAt(offset + 2)) << 16) |
            (static_cast<std::uint32_t>(byteAt(offset + 3)) << 24)
        );
    };

    const auto matches = [&](std::size_t offset, const char* value) {
        for (std::size_t i = 0; i < 4; ++i) {
            if (header[offset + i] != value[i]) {
                return false;
            }
        }
        return true;
    };

    if (!matches(0, "RIFF") ||
        !matches(8, "WAVE") ||
        !matches(12, "fmt ") ||
        !matches(36, "data") ||
        readU32(16) != 16 ||
        readU16(20) != 1 ||
        readU16(22) != 1 ||
        readU16(34) != 16) {

        std::cerr << "Unsupported WAV format\n";
        return 1;
    }

    const std::uint32_t sampleRate = readU32(24);
    const std::uint32_t dataSize = readU32(40);

    if (sampleRate == 0 ||
        dataSize == 0 ||
        dataSize % sizeof(std::int16_t) != 0) {

        std::cerr << "Invalid WAV data\n";
        return 1;
    }

    const std::size_t sampleCount =
        dataSize / sizeof(std::int16_t);

    double peak = 0.0;
    double sumOfSquares = 0.0;

    // Decode PCM16 and measure signal levels.
    for (std::size_t i = 0; i < sampleCount; ++i) {
        unsigned char bytes[2];

        if (!file.read(reinterpret_cast<char*>(bytes), 2)) {
            std::cerr << "Unexpected end of WAV data\n";
            return 1;
        }

        const std::uint16_t raw =
            static_cast<std::uint16_t>(bytes[0]) |
            (static_cast<std::uint16_t>(bytes[1]) << 8);

        const std::int32_t signedSample =
            raw < 0x8000u
                ? static_cast<std::int32_t>(raw)
                : static_cast<std::int32_t>(raw) - 65536;

        const double sample =
            static_cast<double>(signedSample) / 32768.0;

        peak = std::max(peak, std::abs(sample));

        sumOfSquares += sample * sample;
    }

    const double rms = std::sqrt(
        sumOfSquares / static_cast<double>(sampleCount)
    );

    const double peakDbFS = amplitudeToDbFS(peak);
    const double rmsDbFS = amplitudeToDbFS(rms);

    std::cout << std::fixed << std::setprecision(6);

    std::cout << "\n=== Day 10: Audio Level Analysis ===\n";
    std::cout << "File:           " << filename << '\n';
    std::cout << "Sample rate:    " << sampleRate << " Hz\n";
    std::cout << "Samples:        " << sampleCount << '\n';
    std::cout << "Duration:       "
              << static_cast<double>(sampleCount) / sampleRate
              << " seconds\n";

    std::cout << "\n=== Measurements ===\n";
    std::cout << "Peak Amplitude: " << peak << '\n';
    std::cout << "Peak Level:     " << peakDbFS << " dBFS\n";
    std::cout << "RMS Amplitude:  " << rms << '\n';
    std::cout << "RMS Level:      " << rmsDbFS << " dBFS\n";

    if (peakDbFS > 0.0) {
        std::cout << "\nWARNING: Peak exceeds 0 dBFS\n";
    }

    return 0;
}

