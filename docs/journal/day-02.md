# Day 02 — Multi-File Compilation and Audio Sample Analysis

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 3, 2026  
**Phase:** 1 — C++ Foundations and Development Tooling  
**Status:** Implementation and testing completed; final documentation commit pending

## 1. Objective

The objective of Day 2 was to develop a modular audio sample analyzer using modern C++20 while maintaining a programming style close to C.

The exercise focused on organizing source code into multiple translation units, separating function declarations from definitions, working with pointers and arrays, performing basic numerical operations, and handling errors through return values.

The project also introduced automated testing with CTest and reinforced the compilation and linking principles studied on Day 1.

## 2. Development Environment

The project continued using the development environment established on Day 1.

| Component | Technology |
|---|---|
| Operating System | Ubuntu Linux |
| Programming Language | C++20 |
| Primary Compiler | Clang |
| Secondary Compiler | GCC |
| Build-System Generator | CMake |
| Build Executor | Ninja |
| Testing | Custom C++ test harness and CTest |
| Editor | Neovim |
| Version Control | Git / GitHub |

Exact tool versions and installation procedures are recorded in [Development Setup](../setup.md).

## 3. Project Structure

The project was expanded to separate the analyzer's public interface, implementation, application entry point, and test code.

```text
tts-engine/
├── CMakeLists.txt
├── include/
│   └── tts/
│       └── audio/
│           └── analyzer.hpp
├── src/
│   ├── main.cpp
│   └── audio/
│       └── analyzer.cpp
├── tests/
│   └── test_analyzer.cpp
└── docs/
    └── journal/
        ├── day-01.md
        └── day-02.md
```

### File Responsibilities

| File | Responsibility |
|---|---|
| `analyzer.hpp` | Declares the analyzer's public functions and provides header guards |
| `analyzer.cpp` | Implements calculations and input validation |
| `main.cpp` | Creates sample data, calls analyzer functions, and displays results |
| `test_analyzer.cpp` | Tests numerical correctness and invalid-input handling |
| `CMakeLists.txt` | Configures the application and test builds |

## 4. Implementation

### 4.1 Analyzer Interface

Three functions were implemented:

```cpp
bool findMaxSample(const float* samples, int count, float* result);

bool findMinSample(const float* samples, int count, float* result);

bool calculateMeanAmplitude(const float* samples, int count, float* result);
```

The functions use a consistent C-style interface.

The `samples` pointer provides read-only access to the sample data. The `count` parameter specifies the number of elements, while the `result` pointer identifies where the computed value will be stored.

The boolean return value communicates whether the operation succeeded.

### 4.2 Maximum and Minimum Samples

The maximum and minimum functions traverse the audio sample array and compare its elements.

An initial implementation incorrectly initialized the maximum result to zero and did not initialize the minimum result before using it.

Both issues were corrected by initializing the output using the first sample, after validating the input.

This makes the functions work correctly for ordinary finite arrays containing positive, negative, or mixed values.

### 4.3 Mean Absolute Amplitude

Mean absolute amplitude was calculated using:

\[
A_{\text{mean}} = \frac{1}{N}\sum_{i=0}^{N-1}|x_i|
\]

Each sample's absolute value is accumulated before the total is divided by the number of samples.

The implementation uses `std::abs()` from `<cmath>`.

This operation provides a basic measure of audio signal magnitude without allowing positive and negative sample values to cancel each other.

### 4.4 Error Handling

The analyzer functions validate the following conditions before accessing memory:

- Sample count must be greater than zero.
- The sample pointer must not be null.
- The result pointer must not be null.

Invalid arguments cause the function to return `false` without modifying the output value.

Error messages were removed from the analyzer implementation so that higher-level code can decide how failures should be reported.

This separates error detection from error handling.

A remaining limitation is that the caller must ensure the sample pointer refers to a valid buffer containing at least `count` elements.

## 5. Experiments

### Experiment 1 — Multi-File Organization

**Objective:** Separate function declarations from their implementations and call those functions from another translation unit.

The analyzer's declarations were placed in `analyzer.hpp`, with implementations in `analyzer.cpp`.

The application includes the header without including the `.cpp` implementation file.

**Observation:** The compiler can use function declarations to check calls, while the linker resolves the corresponding definitions from separately compiled object files.

### Experiment 2 — Manual Compilation and Linking

**Objective:** Compile the application and analyzer separately, then link their object files into one executable.

**Commands:**

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude \
    -c src/main.cpp -o main.o

clang++ -std=c++20 -Wall -Wextra -Wpedantic -Iinclude \
    -c src/audio/analyzer.cpp -o analyzer.o

clang++ main.o analyzer.o -o tts-engine

./tts-engine
```

**Result:** Manual compilation and linking were reported successful.

**Concepts Learned:**

- `-c` generates object files without linking.
- `-Iinclude` adds the project's header directory to the compiler's search paths.
- Each `.cpp` file is compiled as a separate translation unit.
- Linking combines object files and resolves external symbols.

### Experiment 3 — CMake and Ninja Build

**Objective:** Reproduce the multi-file build using CMake and Ninja.

**Commands:**

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++

cmake --build build

./build/tts-engine
```

**Expected Application Output:**

```text
TTS Engine initialized!
Audio Sampler Analyzer
----------------------
Maximum: 0.8
Minimum: -0.5
Mean: 0.38
```

**Result:** The CMake/Ninja project build was successfully verified.

The build configuration includes both `main.cpp` and `analyzer.cpp`, along with the project's public header search directory.

### Experiment 4 — Automated Analyzer Testing

**Objective:** Verify numerical results, input validation, and error-handling behavior.

A separate test executable was created using the same analyzer implementation.

The test harness uses a function pointer type compatible with all three analyzer functions:

```cpp
using AnalyzerFunction = bool (*)(const float*, int, float*);
```

This allows common test logic to exercise different analyzer operations without duplicating the testing procedure.

The test suite contains eight cases for each analyzer function:

| Test Case | Expected Behavior |
|---|---|
| Normal mixed samples | Correct result |
| All-negative samples | Correct result |
| All-positive samples | Correct result |
| Single sample | Correct result |
| Null sample pointer | Return `false` |
| Null result pointer | Return `false` |
| Zero count | Return `false` |
| Negative count | Return `false` |

**Commands:**

```bash
./build/test-analyzer

ctest --test-dir build --output-on-failure
```

**CTest Result:**

```text
1/1 Test #1: AudioAnalyzerTests ... Passed

100% tests passed, 0 tests failed out of 1
```

CTest registers the complete test executable as one test. The executable internally performs 24 individual checks.

**Internal Test Summary:** Verify and record the exact output from `./build/test-analyzer`.

## 6. Problems Encountered

### Problem 1 — Passing a Pointer to an Entire Array

An early function call passed `&samples` rather than `samples`.

These expressions have different types. The array expression ordinarily converts to a pointer to its first element, whereas applying `&` to the array produces a pointer to the entire array.

The call was corrected to match the analyzer's `const float*` parameter.

**Lesson:** Pointer types and array-to-pointer conversion matter when designing function interfaces.

### Problem 2 — Incorrect Maximum Initialization

The maximum function initially initialized its result to zero.

This produced incorrect behavior for arrays containing only negative samples.

**Resolution:** Initialize the current maximum from the first valid array element.

### Problem 3 — Uninitialized Minimum Result

The minimum function initially compared samples against the output value without assigning it first.

**Resolution:** Initialize the result using the first valid sample before the comparison loop.

**Lesson:** Reading an uninitialized floating-point value can produce undefined behavior.

### Problem 4 — Incorrect Interpretation of an Expected Failure

An initial invalid-input test treated the analyzer returning `false` as a test failure, even though failure was the expected behavior.

**Resolution:** Test both the returned status and whether the output remains unchanged.

This reinforced the distinction between a function failing as intended and an automated test failing.

## 7. Connections to Computer Systems: A Programmer's Perspective

### Separate Compilation and Linking

Each source file is translated independently. The linker resolves function references between the resulting object files.

### Pointers and Memory Access

The analyzer uses explicit pointers to access input samples and store output values.

The exercise demonstrated the differences between pointers to elements, pointers to arrays, and pointers used as output parameters.

### Data Representation

Audio samples are represented using 32-bit floating-point values on the current development platform.

Understanding sample types and memory usage will become important when working with larger audio buffers.

### Error Propagation

Analyzer functions report invalid input through boolean return values, allowing the caller to determine how to handle failures.

### Function Pointers

The test harness demonstrates how functions with compatible types can be invoked indirectly through a function pointer.

The function pointer determines how a function can be called, but not what mathematical operation it performs.

## 8. Personal Reflection

### What did I learn?

Todays focus started with learning about the syntax of the parts of C++ I want to use. There are a lot of features in C++ that can get very overwhelming. It is very easy to get into a rabit hole and lose track of time. My advice is to pick a small set of features you plan on using and just stick with them and get good at them. The next focus went to understanding build systems and testing in C++ and CMake. 

### What was most challenging?

What was challenging was trying to figure out the build system and how it interacted with the testing environment. 

## 9. Next Steps — Day 3

Day 3 will build on the concepts introduced in the analyzer exercise.

Planned topics:

- Reusable static libraries.
- `add_library()` in CMake.
- `target_link_libraries()` and target dependencies.
- The distinction between an application executable and a reusable library.
- Organizing project components for independent testing.

## 10. Day 2 Completion Criteria

- [x] Create the analyzer interface and implementation.
- [x] Integrate the analyzer into `main.cpp`.
- [x] Implement maximum, minimum, and mean absolute amplitude calculations.
- [x] Complete invalid-input tests.
- [x] Manually compile and link multiple source files.
- [x] Verify the CMake/Ninja build.
- [x] Run the CTest suite successfully.
- [x] Complete personal reflections.
- [x] Commit and push Day 2 changes.

**Final Git Commit:** _Record after committing._

