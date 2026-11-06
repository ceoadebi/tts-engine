cmake_minimum_required(VERSION 3.20)

project(
    TTSEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXSTENSIONS OFF)

add_library(tts_audio STATIC
    src/audio/analyzer.cpp
    src/audio/audio_buffer.cpp
)

target_include_directories(tts_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(tts_audio PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
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

# TESTING 
enable_testing()

# TESTING ANALYZER 
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

#TESTING AUDIOBUFFER
add_test(
    NAME AudioAnalyzerTests
    COMMAND test-analyzer
)

add_executable(test-audio-buffer 
    tests/test_audio_buffer.cpp
)

target_link_libraries(test-audio-buffer PRIVATE
    tts_audio
)

add_test(
    NAME AudioBufferTests
    COMMAND test-audio-buffer
)

target_include_directories(test-analyzer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

