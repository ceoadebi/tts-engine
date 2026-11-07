#ifndef TTS_AUDIO_WAVE_WRITER_HPP
#define TTS_AUDIO_WAVE_WRITER_HPP

#include <cstdint>
#include <filesystem>
#include <span>

bool writeWavPCM16(
    const std::filesystem::path& path,
    std::span<const float> samples,
    std::uint32_t sampleRate,
    std::uint16_t channels
);
#endif
