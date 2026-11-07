
#include "tts/audio/wav_writer.hpp"
#include "tts/audio/pcm.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <filesystem>
#include <span> 

namespace {

// Write a 32-bit unsigned integer in little-endian order.
void writeUint32LE(std::ofstream& file, std::uint32_t value) {
    for (int i{0}; i < 4; ++i) {
        const std::uint8_t byte = static_cast<std::uint8_t>(
            (value >> (8 * i)) & 0xFFu
        );

        file.write(
            reinterpret_cast<const char*>(&byte),
            sizeof(byte)
        );
    }
}

// Write a 16-bit unsigned integer in little-endian order.
void writeUint16LE(std::ofstream& file, std::uint16_t value) {
    for (int i{0}; i < 2; ++i) {
        const std::uint8_t byte = static_cast<std::uint8_t>(
            (value >> (8 * i)) & 0xFFu
        );

        file.write(
            reinterpret_cast<const char*>(&byte),
            sizeof(byte)
        );
    }
}

} // namespace

bool writeWavPCM16(
    const std::filesystem::path& path,
    std::span<const float> samples,
    std::uint32_t sampleRate,
    std::uint16_t channels
) {
    // 1. Validate audio parameters.
    if (sampleRate == 0 || channels == 0) {
        return false;
    }

    // Every frame must contain one sample per channel.
    if (samples.size() % channels != 0) {
        return false;
    }

    // 2. Calculate PCM16 data size without overflowing.
    constexpr std::size_t bytesPerSample = sizeof(std::int16_t);
    constexpr std::uint32_t headerOverhead = 36;

    if (samples.size() >
        (std::numeric_limits<std::uint32_t>::max() -
         headerOverhead) / bytesPerSample) {
        return false;
    }

    const std::size_t dataBytes =
        samples.size() * bytesPerSample;

    const std::size_t chunkSize =
        headerOverhead + dataBytes;

    // 3. Calculate BlockAlign and ByteRate safely.
    const std::uint32_t blockAlignValue =
        static_cast<std::uint32_t>(channels) * bytesPerSample;

    if (blockAlignValue >
        std::numeric_limits<std::uint16_t>::max()) {
        return false;
    }

    const auto blockAlign =
        static_cast<std::uint16_t>(blockAlignValue);

    if (sampleRate >
        std::numeric_limits<std::uint32_t>::max() / blockAlign) {
        return false;
    }

    const std::uint32_t byteRate =
        sampleRate * blockAlign;

    // Validated above: values fit in the WAV header fields.
    const auto riffSize =
        static_cast<std::uint32_t>(chunkSize);

    const auto pcmDataSize =
        static_cast<std::uint32_t>(dataBytes);

    // 4. Open the file in binary mode.
    std::ofstream file{path, std::ios::binary | std::ios::trunc};

    if (!file) {
        return false;
    }

    // 5. RIFF header (12 bytes).
    file.write("RIFF", 4);
    writeUint32LE(file, riffSize);
    file.write("WAVE", 4);

    // 6. Format chunk (24 bytes).
    file.write("fmt ", 4);
    writeUint32LE(file, 16);       // PCM format chunk size
    writeUint16LE(file, 1);        // Integer PCM
    writeUint16LE(file, channels);
    writeUint32LE(file, sampleRate);
    writeUint32LE(file, byteRate);
    writeUint16LE(file, blockAlign);
    writeUint16LE(file, 16);       // Bits per sample

    // 7. Data chunk header (8 bytes).
    file.write("data", 4);
    writeUint32LE(file, pcmDataSize);

    // 8. Convert and write audio samples.
    for (float sample : samples) {
        const std::int16_t pcm = floatToPCM16(sample);

        writeUint16LE(
            file,
            static_cast<std::uint16_t>(pcm)
        );
    }

    // 9. Flush and close the output file.
    file.close();

    return static_cast<bool>(file);
}

