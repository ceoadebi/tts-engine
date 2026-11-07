
#include "tts/audio/wav_writer.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

namespace {

int testsPassed = 0;
int testsFailed = 0;

void check(bool condition, const char* description) {
    if (condition) {
        ++testsPassed;
        std::cout << "[PASS] " << description << '\n';
    } else {
        ++testsFailed;
        std::cerr << "[FAIL] " << description << '\n';
    }
}

// Read an entire file as unsigned bytes.
std::vector<std::uint8_t> readBytes(
    const std::filesystem::path& path
) {
    std::ifstream file{path, std::ios::binary};

    if (!file) {
        return {};
    }

    std::vector<std::uint8_t> bytes;

    for (std::istreambuf_iterator<char> it{file}, end;
         it != end; ++it) {
        bytes.push_back(static_cast<std::uint8_t>(*it));
    }

    return bytes;
}

// Check a sequence of bytes at a specific offset.
bool matches(
    const std::vector<std::uint8_t>& bytes,
    std::size_t offset,
    std::initializer_list<std::uint8_t> expected
) {
    if (offset > bytes.size() ||
        expected.size() > bytes.size() - offset) {
        return false;
    }

    std::size_t i = offset;

    for (std::uint8_t value : expected) {
        if (bytes[i++] != value) {
            return false;
        }
    }

    return true;
}

} // namespace

int main() {
    namespace fs = std::filesystem;

    // Test 1: Generate a mono WAV file.
    constexpr std::array<float, 5> monoSamples{
        0.0f, 0.5f, -0.5f, 1.0f, -1.0f
    };

    const fs::path monoPath{"test_mono.wav"};

    check(
        writeWavPCM16(monoPath, monoSamples, 48000, 1),
        "Write mono PCM16 WAV"
    );

    const auto mono = readBytes(monoPath);

    // 44-byte header + 5 samples * 2 bytes.
    check(mono.size() == 54, "Mono file size is 54 bytes");

    // RIFF/WAVE header.
    check(matches(mono, 0, {'R', 'I', 'F', 'F'}),
          "RIFF identifier");

    check(matches(mono, 4, {46, 0, 0, 0}),
          "RIFF chunk size is 46");

    check(matches(mono, 8, {'W', 'A', 'V', 'E'}),
          "WAVE identifier");

    // Format chunk.
    check(matches(mono, 12, {'f', 'm', 't', ' '}),
          "fmt chunk identifier");

    check(matches(mono, 16, {16, 0, 0, 0}),
          "PCM format chunk size is 16");

    check(matches(mono, 20, {1, 0}),
          "Audio format is integer PCM");

    check(matches(mono, 22, {1, 0}),
          "Mono channel count");

    check(matches(mono, 24, {0x80, 0xBB, 0, 0}),
          "Sample rate is 48000");

    check(matches(mono, 28, {0x00, 0x77, 0x01, 0x00}),
          "Byte rate is 96000");

    check(matches(mono, 32, {2, 0}),
          "Block alignment is 2");

    check(matches(mono, 34, {16, 0}),
          "Bits per sample is 16");

    // Data chunk.
    check(matches(mono, 36, {'d', 'a', 't', 'a'}),
          "Data chunk identifier");

    check(matches(mono, 40, {10, 0, 0, 0}),
          "PCM data size is 10");

    // Samples:
    //  0.0 ->      0 -> 00 00
    //  0.5 ->  16384 -> 00 40
    // -0.5 -> -16384 -> 00 C0
    //  1.0 ->  32767 -> FF 7F
    // -1.0 -> -32768 -> 00 80
    check(
        matches(mono, 44, {
            0x00, 0x00,
            0x00, 0x40,
            0x00, 0xC0,
            0xFF, 0x7F,
            0x00, 0x80
        }),
        "PCM16 samples are encoded correctly"
    );

    // Test 2: Generate a stereo WAV file.
    // Interleaved as L0, R0, L1, R1.
    constexpr std::array<float, 4> stereoSamples{
        1.0f, -1.0f,
        0.5f, -0.5f
    };

    const fs::path stereoPath{"test_stereo.wav"};

    check(
        writeWavPCM16(stereoPath, stereoSamples, 48000, 2),
        "Write stereo PCM16 WAV"
    );

    const auto stereo = readBytes(stereoPath);

    check(stereo.size() == 52,
          "Stereo file size is 52 bytes");

    check(matches(stereo, 22, {2, 0}),
          "Stereo channel count");

    check(matches(stereo, 28, {0x00, 0xEE, 0x02, 0x00}),
          "Stereo byte rate is 192000");

    check(matches(stereo, 32, {4, 0}),
          "Stereo block alignment is 4");

    check(matches(stereo, 40, {8, 0, 0, 0}),
          "Stereo PCM data size is 8");

    check(
        matches(stereo, 44, {
            0xFF, 0x7F,
            0x00, 0x80,
            0x00, 0x40,
            0x00, 0xC0
        }),
        "Stereo samples are correctly interleaved"
    );

    // Test 3: Invalid sample rate.
    check(
        !writeWavPCM16("invalid_rate.wav", monoSamples, 0, 1),
        "Reject zero sample rate"
    );

    // Test 4: Invalid channel count.
    check(
        !writeWavPCM16("invalid_channels.wav", monoSamples, 48000, 0),
        "Reject zero channels"
    );

    // Test 5: Incomplete stereo frame.
    check(
        !writeWavPCM16("incomplete.wav", monoSamples, 48000, 2),
        "Reject incomplete stereo frame"
    );

    // Test 6: Invalid output directory.
    // Ensure this directory does not exist.
    const fs::path missingDir{"wav_test_missing_directory"};
    std::error_code ec;
    fs::remove_all(missingDir, ec);

    if (ec || fs::exists(missingDir)) {
        check(false, "Prepare nonexistent output directory");
    } else {
        check(
            !writeWavPCM16(
                missingDir / "invalid.wav",
                monoSamples,
                48000,
                1
            ),
            "Reject nonexistent output directory"
        );
    }

    // Clean up generated WAV files.
    fs::remove(monoPath, ec);
    fs::remove(stereoPath, ec);

    std::cout << "\nTests passed: " << testsPassed << '\n';
    std::cout << "Tests failed: " << testsFailed << '\n';

    return testsFailed == 0 ? 0 : 1;
}

