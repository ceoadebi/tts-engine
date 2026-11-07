
cmake_minimum_required(VERSION 3.20)

project(
    TTSEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

# ----------------------------------------
# C++20 Configuration
# ----------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------------------------------
# Audio Static Library
# ----------------------------------------

add_library(tts_audio STATIC
    src/audio/analyzer.cpp
    src/audio/audio_buffer.cpp
    src/audio/pcm.cpp
    src/audio/wav_writer.cpp
)

# Public headers are available to all
# targets that link against tts_audio
target_include_directories(tts_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(tts_audio PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ----------------------------------------
# Main Application
# ----------------------------------------

add_executable(tts-engine
    src/main.cpp
)

target_link_libraries(tts-engine PRIVATE
    tts_audio
)

target_compile_options(tts-engine PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# ----------------------------------------
# Testing
# ----------------------------------------

enable_testing()

# Audio Analyzer Tests
add_executable(test-analyzer
    tests/test_analyzer.cpp
)

target_link_libraries(test-analyzer PRIVATE
    tts_audio
)

target_compile_options(test-analyzer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME AudioAnalyzerTests
    COMMAND test-analyzer
)

# AudioBuffer Tests
add_executable(test-audio-buffer
    tests/test_audio_buffer.cpp
)

target_link_libraries(test-audio-buffer PRIVATE
    tts_audio
)

target_compile_options(test-audio-buffer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME AudioBufferTests
    COMMAND test-audio-buffer
)

# PCM16 Conversion Tests
add_executable(test-pcm
    tests/test_pcm.cpp
)

target_link_libraries(test-pcm PRIVATE
    tts_audio
)

target_compile_options(test-pcm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME PCM16Tests
    COMMAND test-pcm
)

# WAV WRITER TESTS

add_executable(test-wav-writer tests/test_wav_writer.cpp)

target_link_libraries(test-wav-writer PRIVATE tts_audio)

target_compile_options(test-wav-writer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(NAME WavWriterTests COMMAND test-wav-writer)
