# C++20 Text-to-Speech Engine

## Overview

The goal of this project is to design and develop a customizable text-to-speech (TTS) engine using modern C++20. The engine will aim to provide control over both the characteristics of 
synthesized voices and the delivery of speech including pitch, timbre, pacing, and pauses.

This project will explore how computer architecture, memory management, program execution, and operating systems concepts influence the design and perfomance of a speech synthesis system. 
Through its development, I aim to gain a deeper understanding of C++, software architecture, digital signal processing, WAV generation, neural network inference, and systems programming.

## Project Objectives
- Develop a modular TTS application using C++20.
- Understand and implement fundamental digital audio processing operations.
- Integrate pretrained neural speech models using ONNX Runtime.
- Support configurable narration characteristics such as speaking rate, pauses, and model-supported voice settings.
- Investigate CPU and GPU inference performance.

## Technology Stack

| Component | Technology |
|---|---|
| Programming Language | C++20 |
| Operating System | Ubuntu Linux |
| Primary Compiler | Clang |
| Secondary Compiler | GCC |
| Build System Generator | CMake |
| Build Executor | Ninja |
| Text Editor | Neovim |
| Debugger | GDB |
| Neural Network Inference | ONNX Runtime (planned) |
| Version Control | Git / GitHub |
| GPU Acceleration | NVIDIA CUDA (optional, planned) |

## Planned Architecture

The engine is intended to follow a modular processing pipeline:

```text
                 Input Text
                      |
                      v
              Text Normalization
                      |
                      v
                Phonemization
                      |
                      v
              Neural TTS Model
               (Voice Controls)
                      |
                      v
               Generated Audio
                      |
                      v
             DSP Post-Processing
               (Audio Controls)
                      |
                      v
              WAV File / Playback
```

The implementation will separate text processing, neural network inference, audio processing, and output generation.

### Core Components

**Text Processing**

Responsible for preparing written text for speech synthesis, including normalization and conversion into model-compatible representations.

**Neural Network Inference**

Responsible for loading pretrained speech synthesis models, preparing model inputs, executing inference, and retrieving generated audio.

**Digital Audio Processing**

Responsible for manipulating audio samples, applying supported effects, adjusting playback characteristics, and managing audio buffers.

**Audio Output**

Responsible for encoding audio into WAV files and potentially supporting playback through audio devices.

**Application Interface**

A command-line interface will provide access to the engine. Reusable interfaces may eventually allow the engine to be integrated into other applications.

## Systems Programming Focus

This project will incorporate concepts from *Computer Systems: A Programmer's Perspective*.

| CS:APP Concept | Project Application |
|---|---|
| Information Representation | PCM formats, floating-point samples, binary WAV headers |
| Machine-Level Programs | Examining compiler-generated machine instructions |
| Processor Architecture | Understanding execution behavior and optimization |
| Memory Hierarchy | Cache-friendly audio buffers and tensor storage |
| Linking | Integrating external libraries and managing dependencies |
| Exceptional Control Flow | Signals, error handling, and process behavior |
| Virtual Memory | Dynamic allocation and memory usage |
| System-Level I/O | File operations, audio output, and model loading |
| Concurrent Programming | Investigating parallel audio processing and inference |

The objective is not only to build a functioning engine but also to understand the computer systems principles underlying its implementation.

## Development Roadmap

### Phase 1 — Development Environment and C++ Foundations

- [ ] Configure and verify the C++20 development environment.
- [ ] Establish the GitHub repository and documentation structure.
- [ ] Understand compilation, linking, CMake, and Ninja.
- [ ] Practice memory management, standard containers, and modern C++ design.
- [ ] Establish reproducible builds and basic testing.

### Phase 2 — Digital Audio Fundamentals

- [ ] Understand PCM representation and sample formats.
- [ ] Generate and manipulate audio buffers.
- [ ] Implement WAV file generation.
- [ ] Explore signal processing operations such as gain, mixing, and fades.
- [ ] Develop tests for audio correctness.

### Phase 3 — Neural Network Fundamentals and Inference

- [ ] Study tensors and fundamental neural network operations.
- [ ] Integrate ONNX Runtime.
- [ ] Load and inspect ONNX models.
- [ ] Execute inference using the CPU.
- [ ] Investigate tensor memory management and performance.

### Phase 4 — Text-to-Speech Integration

- [ ] Implement text normalization.
- [ ] Integrate phonemization.
- [ ] Prepare inputs for a compatible speech synthesis model.
- [ ] Generate intelligible speech from text.
- [ ] Export synthesized speech to WAV files.

### Phase 5 — Voice Customization and Narration

- [ ] Investigate voice characteristics and synthesis-time controls.
- [ ] Implement configurable speech delivery where supported.
- [ ] Develop DSP post-processing functionality.
- [ ] Explore pitch, pacing, pauses, and timbre customization.
- [ ] Evaluate speech quality and processing tradeoffs.

### Phase 6 — Optimization, Testing, and Release

- [ ] Benchmark inference speed and memory usage.
- [ ] Investigate parallel processing and optional GPU acceleration.
- [ ] Improve robustness and error handling.
- [ ] Expand automated testing.
- [ ] Complete reproducibility documentation.
- [ ] Prepare an initial release.

## Getting Started

**Project Status:** Initial development — Phase 1.

The project is currently focused on establishing the development environment and foundational C++ tooling.

Build and execution instructions will be added and verified as the first implementation is completed.

See [Development Setup](docs/setup.md) for environment configuration and dependency information.

## Repository Structure

```text
tts-engine/
├── README.md
├── LICENSE
├── .gitignore
├── CMakeLists.txt
├── CMakePresets.json
├── include/
│   └── tts/
├── src/
│   └── main.cpp
├── tests/
├── benchmarks/
├── docs/
│   ├── setup.md
│   ├── architecture.md
│   ├── roadmap.md
│   ├── journal/
│   ├── decisions/
│   └── experiments/
├── models/
└── scripts/
```

This structure represents the planned organization. Directories and files will be introduced as needed throughout development.

Generated build artifacts, temporary audio output, and large model weights will generally remain outside version control.

## Documentation

A primary objective of this project is to document the development process so that the work can be studied, reproduced, and extended.

Documentation will include:

- [Development Setup](docs/setup.md) — Environment configuration and dependencies.
- [Architecture](docs/architecture.md) — System components and design.
- [Development Roadmap](docs/roadmap.md) — Milestones and progress tracking.
- [Engineering Journal](docs/journal/) — Daily progress, challenges, and lessons.
- [Architecture Decisions](docs/decisions/) — Records of important engineering decisions.
- [Experiments](docs/experiments/) — Hypotheses, procedures, measurements, and results.

Documentation will be expanded as implementation progresses.

## Project Status

**Current Phase:** Phase 1 — Development Environment and C++ Foundations.

The engine is in early development. Audio generation, speech synthesis, voice customization, and neural inference are planned capabilities rather than completed features.

## License

This project is licensed under the MIT License.

See [LICENSE](LICENSE) for details.

Third-party dependencies and pretrained speech models may be governed by separate licenses.
