# Day 01 — C++20 Development Environment

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 2, 2026  
**Phase:** 1 — Development Environment and C++ Foundations  
**Status:** Implementation and setup completed; documentation and final commit pending

## 1. Objective

The objective of Day 1 was to establish a reproducible C++20 development environment and understand the fundamental process of transforming C++ source code into an executable.

The primary tools investigated were Clang, GCC, CMake, Ninja, Git, and Neovim.

This work establishes the development foundation for implementing the text-to-speech engine.

## 2. Development Environment

| Component | Configuration |
|---|---|
| Operating System | Ubuntu Linux |
| Programming Language | C++20 |
| Primary Compiler | Clang |
| Secondary Compiler | GCC |
| Build-System Generator | CMake |
| Build Executor | Ninja |
| Text Editor | Neovim |
| Debugger | GDB |
| Version Control | Git / GitHub |

**Environment-specific details to record:**

- Ubuntu version:
- Linux kernel version:
- Clang version:
- GCC version:
- CMake version:
- Ninja version:
- Git commit hash:

These details are important for reproducing the experiments on another machine.

## 3. Tasks Completed

- [x] Verified Ubuntu and the development environment.
- [x] Verified the required development tools.
- [x] Established the GitHub repository.
- [x] Created the project README and selected the MIT License.
- [x] Created and compiled the initial C++20 program.
- [x] Investigated preprocessing, compilation, assembly, and linking.
- [x] Configured and built the project using CMake and Ninja.
- [x] Created `docs/setup.md`.
- [x] Complete this engineering journal and commit the documentation.

## 4. Experiments

### Experiment 1 — Manual C++20 Compilation

**Objective:** Compile and execute a C++20 program using Clang.

**Command:**

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic src/main.cpp -o tts-engine
```

**Expected Result:**

Clang produces an executable that runs successfully and prints the initialization message.

**Observed Result:**

The program was successfully compiled and executed.

**Concepts Learned:**

Compilation translates source code into machine code. Linking produces an executable from object files and required libraries. Execution occurs when the operating system loads and starts the program.

### Experiment 2 — Investigating the Compilation Pipeline

**Objective:** Understand the four principal stages of the compilation process.

| Stage | Description | Output |
|---|---|---|
| Preprocessing | Processes directives such as `#include` | Preprocessed source |
| Compilation | Translates preprocessed C++ into assembly | `.s` file |
| Assembly | Converts assembly into machine-code object files | `.o` file |
| Linking | Resolves symbols and constructs an executable | Executable |

**Commands investigated:**

```bash
clang++ -std=c++20 -E src/main.cpp -o main.ii
clang++ -std=c++20 -S src/main.cpp -o main.s
clang++ -std=c++20 -c src/main.cpp -o main.o
clang++ main.o -o tts-engine
```

**Concepts Learned:**

- The `-E` flag stops after preprocessing.
- The `-S` flag stops after generating assembly.
- The `-c` flag produces an object file without linking.
- The linker resolves references to symbols defined in other object files and libraries.
- Inspecting assembly can help investigate machine-level behavior and possible optimization opportunities.

**Observation:**

The C++ source, assembly file, object file, and executable represent different stages of transforming a program into machine-executable instructions.

### Experiment 3 — CMake and Ninja

**Objective:** Configure and build the project using a build system rather than manually invoking Clang.

**Configuration Command:**

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
```

**Build Command:**

```bash
cmake --build build
```

**Execution Command:**

```bash
./build/tts-engine
```

**Observed Result:**

The project successfully configured and built after resolving the initial cache error.

**Concepts Learned:**

CMake generates build-system instructions from project configuration files, while Ninja executes those instructions and tracks build dependencies.

This separation helps automate compilation and linking as the project grows.

## 5. Problems Encountered

### Problem 1 — Git Push Rejected

**Issue:**

The remote GitHub repository contained changes not yet integrated into the local branch.

**Investigation:**

Reviewed the relationship between local and remote Git branches and the purpose of fetching and merging changes.

**Resolution:**

The local and remote repositories were successfully synchronized.

**Lesson Learned:**

Git protects remote history from being overwritten by non-fast-forward pushes. Understanding repository history is important before merging or pushing changes.

### Problem 2 — CMake Could Not Load Cache

**Error:**

```text
Error: could not load cache
```

**Cause:**

CMake could not access a valid configuration cache for the requested build directory.

**Resolution:**

Configured the project before executing the build command.

**Lesson Learned:**

CMake configuration and build execution are separate operations. The configuration step creates the build-system files required by Ninja.

## 6. CS:APP Connections

Several concepts from *Computer Systems: A Programmer's Perspective* were introduced during Day 1.

**Program Translation**

The development process demonstrated how high-level C++ source code progresses through preprocessing, compilation, assembly, and linking.

**Machine-Level Representation**

Generating assembly code introduced the relationship between C++ statements and processor instructions.

**Linking and Symbol Resolution**

Object files contain compiled code and information needed to resolve references between different components.

**Memory Representation**

Preliminary discussions of stack and heap allocation highlighted how data representation and storage duration affect program design.

These concepts will become increasingly important as the engine incorporates audio buffers, external libraries, and neural network inference.

## 7. Reflection

**What did I learn today?**
This process can be as easy or as hard as you want it to be. My goal is to not use AI for critical thinking and to develop my skills in system architecture design. It is easy to make AI do it all for your but that is how you fail.

**What was most challenging?**

The issues with Git are fairly easily resolved with google searches. Making my own CMakelist and using it is very difficult to start with. It something that must be taken slowly.

**What would I do differently next time?**

It's too early to say anything that I'd do differently. I haven't done much to begin with. 

**What questions remain unanswered?**

How much can CMake do in the build system? How much control does it have?

## 8. Next Steps — Day 2

Day 2 will focus on multi-file compilation and linking.

Planned investigations:

- Understand the relationship between header and source files.
- Separate declarations from definitions.
- Compile multiple translation units.
- Investigate external symbol resolution.
- Extend the CMake configuration to support multiple source files.

## 9. Day 1 Completion Criteria

- [x] Development tools verified.
- [x] Initial C++20 program compiled and executed.
- [x] CMake/Ninja build verified.
- [x] GitHub repository synchronized.
- [x] Reproducible setup documentation written.
- [x] Personal reflections completed.
- [x] Final documentation committed and pushed.

**Final Git Commit:** _To be recorded after committing._
