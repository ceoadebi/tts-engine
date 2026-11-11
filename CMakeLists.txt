
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
    src/audio/sine_generator.cpp
    src/audio/envelope.cpp
    src/audio/mixer.cpp
    src/audio/normalizer.cpp
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

#SINE WAVE GENERATOR TESTS
add_executable(test-sine-generator
    tests/test_sine_generator.cpp
)

target_link_libraries(test-sine-generator PRIVATE tts_audio)

target_compile_options(test-sine-generator PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME SineGeneratorTests
    COMMAND test-sine-generator
)

add_executable(day06-sine-wave 
    experiments/day06_sine_wave.cpp
)

target_link_libraries(day06-sine-wave PRIVATE tts_audio)

#ENVELOPE TESTS
add_executable(test-envelope
    tests/test_envelope.cpp
)

target_link_libraries(test-envelope PRIVATE tts_audio)

target_compile_options(test-envelope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME EnvelopeTests
    COMMAND test-envelope
)

add_executable(day07-envelope
    experiments/day07_envelope.cpp
)

target_link_libraries(day07-envelope PRIVATE tts_audio)

target_compile_options(day07-envelope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

#MIXER TESTS
add_executable(test-mixer
    tests/test_mixer.cpp
)

target_link_libraries(test-mixer PRIVATE tts_audio)

target_compile_options(test-mixer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME MixerTests
    COMMAND test-mixer
)
add_executable(day08-mixing experiments/day08_mixing.cpp)
target_link_libraries(day08-mixing PRIVATE tts_audio)

#NORMALIZER TESTS
add_executable(test-normalizer
    tests/test_normalizer.cpp
)

target_link_libraries(test-normalizer PRIVATE tts_audio)

target_compile_options(test-normalizer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_test(
    NAME NormalizerTests
    COMMAND test-normalizer
)

add_executable(day09-normalization
    experiments/day09_normalization.cpp
)

target_link_libraries(day09-normalization PRIVATE tts_audio)
