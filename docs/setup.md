# Development Environment Setup

## 1. Overview

This document describes how to configure and reproduce the development environment for the C++20 Text-to-Speech (TTS) Engine.

The initial development environment uses Ubuntu Linux, Clang, CMake, Ninja, Git, and Neovim.

The purpose of this setup is to establish a reproducible development workflow before implementing audio processing and neural network inference.

## 2. Development Tools

| Tool | Purpose |
|---|---|
| Ubuntu Linux | Development operating system |
| Clang | Primary C++20 compiler |
| GCC | Secondary C++ compiler |
| CMake | Build-system configuration and generation |
| Ninja | Build execution |
| GDB | Debugging |
| Git | Version control |
| Neovim | Source code editor |

## 3. Update the Package Manager

Update the available Ubuntu package lists:

```bash
sudo apt update
```

## 4. Install Development Tools

Install the required development tools using Ubuntu's package manager:

```bash
sudo apt install clang g++ cmake ninja-build gdb git neovim
```

The `g++` package provides the GNU C++ compiler, while `ninja-build` provides the `ninja` executable.

## 5. Verify Tool Installation

Check the installed versions:

```bash
clang++ --version
g++ --version
cmake --version
ninja --version
gdb --version
git --version
nvim --version
```

Record the version numbers when documenting the development environment.

For additional system information:

```bash
uname -a
cat /etc/os-release
```

## 6. Obtain the Project Repository

Clone the project from GitHub:

```bash
git clone https://github.com/ceoadebi/tts-engine.git
```

Enter the project directory:

```bash
cd tts-engine
```

Check the repository's current state:

```bash
git status
```

Verify the configured remote:

```bash
git remote -v
```

The repository contains the project source code, CMake configuration, and documentation.

## 7. Manual Compilation with Clang

Before using CMake, compile the initial C++20 program manually to understand the compilation process.

Compile `src/main.cpp`:

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic src/main.cpp -o tts-engine
```

Execute the resulting program:

```bash
./tts-engine
```

Expected output:

```text
TTS Engine Initialized
```

The output assumes the initial program prints this message.

## 8. Investigating the Compilation Pipeline

The compilation process consists of four main stages:

1. Preprocessing
2. Compilation
3. Assembly
4. Linking

### 8.1 Preprocessing

Use the `-E` flag to stop after preprocessing:

```bash
clang++ -std=c++20 -E src/main.cpp -o main.ii
```

The resulting `main.ii` file contains the preprocessed translation unit.

### 8.2 Assembly Generation

Use the `-S` flag to generate assembly code:

```bash
clang++ -std=c++20 -S src/main.cpp -o main.s
```

Inspect the assembly file:

```bash
nvim main.s
```

The assembly output can be used to investigate compiler-generated machine-level operations.

### 8.3 Object File Generation

Use the `-c` flag to compile and assemble without linking:

```bash
clang++ -std=c++20 -c src/main.cpp -o main.o
```

Inspect the object file format:

```bash
file main.o
```

### 8.4 Linking

Link the object file to produce an executable:

```bash
clang++ main.o -o tts-engine
```

Execute the program:

```bash
./tts-engine
```

The linker resolves external symbols and produces the executable from object files and required libraries.

## 9. Configure the Project with CMake and Ninja

CMake is used to configure the project and generate build instructions for Ninja.

From the root of the repository:

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
```

The command options are:

| Option | Purpose |
|---|---|
| `-S .` | Specify the current directory as the source directory |
| `-B build` | Specify the build directory |
| `-G Ninja` | Select Ninja as the build generator |
| `-DCMAKE_CXX_COMPILER=clang++` | Select Clang as the C++ compiler |

Successful configuration should produce a `CMakeCache.txt` file and a `build.ninja` file inside the build directory.

## 10. Build the Project

After configuration, run:

```bash
cmake --build build
```

Execute the application:

```bash
./build/tts-engine
```

The build directory contains generated build files, intermediate object files, and the executable. It should remain excluded from Git version control.

## 11. Troubleshooting

### Error: Could Not Load Cache

During the initial setup, the following error was encountered:

```text
Error: could not load cache
```

This occurred when CMake attempted to build without an accessible, valid configuration cache in the specified build directory.

The issue was resolved by configuring the project before building:

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
```

Then:

```bash
cmake --build build
```

If an existing build directory has an invalid configuration, a new build directory can be used:

```bash
cmake -S . -B build-clean -G Ninja -DCMAKE_CXX_COMPILER=clang++
cmake --build build-clean
```

### Git Push Rejected

A Git push was initially rejected because the remote branch contained commits that were not integrated into the local branch.

The following commands were discussed for diagnosing the issue:

```bash
git status
git fetch origin
git log --oneline --graph --decorate --all
```

After resolving the branch-history issue, the repository was successfully synchronized with GitHub.

## 12. Git Workflow

After making changes, inspect the repository:

```bash
git status
```

Stage the desired changes:

```bash
git add .
```

Commit them:

```bash
git commit -m "Document Day 1 development environment"
```

Push to GitHub:

```bash
git push origin main
```

Before committing, verify that generated build files and temporary outputs are excluded by `.gitignore`.

## 13. Reproducing the Initial Build

Once the dependencies are installed, a developer should be able to reproduce the initial build with:

```bash
git clone https://github.com/ceoadebi/tts-engine.git
cd tts-engine
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
cmake --build build
./build/tts-engine
```

This sequence represents the baseline development workflow.

## 14. Development Status

**Day 1 — Initial Environment Setup**

- Ubuntu environment verified.
- Development tools verified.
- Git repository initialized and synchronized.
- Initial C++20 application compiled successfully.
- CMake and Ninja build process verified.
- Compiler pipeline investigated.
- Reproducibility documentation established.

Future updates will document the introduction of automated testing, audio processing libraries, ONNX Runtime, and additional dependencies.
