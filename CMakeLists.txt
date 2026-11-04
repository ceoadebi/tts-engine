cmake_minimum_required(VERSION 3.20)

project(
    TTSEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXSTENSIONS OFF)

add_executable(tts-engine
    src/main.cpp
    src/audio/analyzer.cpp
)

target_include_directories(tts-engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(tts-engine PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

enable_testing()

add_executable(test-analyzer
    tests/test_analyzer.cpp
    src/audio/analyzer.cpp
)

target_include_directories(test-analyzer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
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

