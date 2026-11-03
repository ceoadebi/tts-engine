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
)

target_compile_options(tts-engine PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

