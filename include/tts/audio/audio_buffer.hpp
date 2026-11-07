// include/tts/audio/audio_buffer.hpp
#ifndef TTS_AUDIO_AUDIO_BUFFER_HPP
#define TTS_AUDIO_AUDIO_BUFFER_HPP

#include <cstddef>
#include <memory>

class AudioBuffer {
private: 
    std::unique_ptr<float[]> m_data;
    std::size_t m_size;

public: 
    explicit AudioBuffer(std::size_t count);

    AudioBuffer(AudioBuffer&& movedBuffer) noexcept;
    AudioBuffer& operator=(AudioBuffer&& other) noexcept;

    float* data();
    std::size_t size() const;
};

#endif 
