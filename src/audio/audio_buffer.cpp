// src/audio/audio_buffer.cpp
#include <iostream>
#include <cstddef>
#include <memory>
#include <utility> 
#include "tts/audio/audio_buffer.hpp"

AudioBuffer::AudioBuffer(std::size_t count) 
    : m_data{new float[count]{}}, 
      m_size{count}
{
}

AudioBuffer::AudioBuffer(AudioBuffer&& movedBuffer) noexcept 
    : m_data{std::move(movedBuffer.m_data)},
      m_size{movedBuffer.m_size}
{
    movedBuffer.m_size = 0;
}

float* AudioBuffer::data() {
    return m_data.get();
}

const float* AudioBuffer::data() const {
    return m_data.get();
}

std::size_t AudioBuffer::size() const {
    return m_size;
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept
{
    if (this != &other) {
        m_data = std::move(other.m_data);
        m_size = other.m_size;
        other.m_size = 0;
    }

    return *this;
}
