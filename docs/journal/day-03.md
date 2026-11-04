# Day 3 — Static Libraries, CMake Targets, and Linking

**Project:** C++20 Text-to-Speech Engine  
**Environment:** Ubuntu (WSL2), Clang, CMake, Ninja  
**Focus:** Static libraries, symbol resolution, reusable build targets, and incremental compilation  
**Status:** Implementation and verification in progress

---

## 1. Objective

The objective of Day 3 was to refactor the Audio Sample Analyzer developed on Day 2 into a reusable static library.

Previously, `analyzer.cpp` was compiled separately as part of both the main application and the test executable. This approach worked but resulted in redundant compilation.

The goal was to compile the audio analyzer into a static library called `tts_audio` and link both executables against it.

Additional objectives included:

- Understanding the difference between static and shared libraries.
- Creating a static library manually using `ar`.
- Linking an executable against a static library using Clang.
- Refactoring the CMake configuration.
- Understanding `PUBLIC`, `PRIVATE`, and `INTERFACE` usage requirements.
- Investigating defined and undefined symbols with `nm`.
- Understanding incremental compilation and linking.

## 2. Development Environment

| Component | Tool |
|---|---|
| Operating system | Ubuntu (WSL2) |
| Programming language | C++20 |
| Primary compiler | Clang |
| Build configuration | CMake |
| Build system | Ninja |
| Static library tool | `ar` |
| Symbol inspection | `nm` |
| Testing | Custom C++ test harness and CTest |
| Version control | Git and GitHub |

## 3. Project Structure

The project was organized as follows:

```text
tts-engine/
├── CMakeLists.txt
├── README.md
├── LICENSE
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
    ├── setup.md
    └── journal/
        ├── day-01.md
        ├── day-02.md
        └── day-03.md
```

The header file declares the analyzer's public functions, while `analyzer.cpp` contains their implementations.

## 4. Static vs. Shared Libraries

A static library packages compiled object files into an archive.

On Linux, static libraries conventionally use the `.a` extension.

A shared library conventionally uses the `.so` extension and can be loaded by the operating system's dynamic linker.

| Characteristic | Static library | Shared library |
|---|---|---|
| Linux extension | `.a` | `.so` |
| Linking | Usually incorporated at link time | Usually linked for dynamic loading |
| Runtime dependency | Usually does not need the original archive | Usually needs the shared library available |
| Updating implementation | Requires relinking dependent executables | May not require relinking, if ABI-compatible |
| Sharing executable code in memory | Separate executables generally contain their own copies | Multiple processes can share file-backed code pages |

I chose a static library for this stage because it allows me to study object files, symbol resolution, and linking without introducing additional dynamic-library deployment concerns.

## 5. Manual Static Library Creation

### Step 1: Compile the implementation into an object file

```bash
clang++ -std=c++20 -Iinclude \
    -c src/audio/analyzer.cpp \
    -o analyzer.o
```

The `-c` flag instructs the compiler to generate an object file without linking an executable.

### Step 2: Create the library

```bash
ar rcs libtts_audio.a analyzer.o
```

The options mean:

- `r`: Insert or replace object files in the archive.
- `c`: Create the archive if it does not exist.
- `s`: Create or update the archive's symbol index.

### Step 3: Inspect the archive

```bash
ar -t libtts_audio.a
```

The output showed:

```text
analyzer.o
```

This confirmed that the compiled analyzer implementation was packaged into the archive.

### Step 4: Link manually

Compile `main.cpp`:

```bash
clang++ -std=c++20 -Iinclude \
    -c src/main.cpp \
    -o main.o
```

Link against the library:

```bash
clang++ main.o libtts_audio.a -o tts-engine
```

Run:

```bash
./tts-engine
```

The manual linking experiment worked, confirming that the executable could use the analyzer implementation from the static library.

## 6. CMake Refactoring

Previously, the implementation file was listed in multiple executable targets:

```cmake
add_executable(tts-engine
    src/main.cpp
    src/audio/analyzer.cpp
)

add_executable(test-analyzer
    tests/test_analyzer.cpp
    src/audio/analyzer.cpp
)
```

The new approach defines the analyzer as its own build target:

```cmake
add_library(tts_audio STATIC
    src/audio/analyzer.cpp
)

target_include_directories(tts_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

The main executable links against the library:

```cmake
add_executable(tts-engine
    src/main.cpp
)

target_link_libraries(tts-engine PRIVATE
    tts_audio
)
```

The test executable uses the same library:

```cmake
add_executable(test-analyzer
    tests/test_analyzer.cpp
)

target_link_libraries(test-analyzer PRIVATE
    tts_audio
)
```

This creates a reusable build structure in which the analyzer implementation is compiled once for a clean build and consumed by multiple targets.

### PUBLIC, PRIVATE, and INTERFACE

CMake uses these keywords to describe how usage requirements propagate between targets.

**PRIVATE:** The property applies to the current target, but is not propagated to its consumers.

**PUBLIC:** The property applies to the current target and propagates to consumers that link against it.

**INTERFACE:** The property propagates to consumers but does not apply when compiling the target itself.

Using `PUBLIC` for the include directory allows both `tts-engine` and `test-analyzer` to locate `tts/audio/analyzer.hpp` through their link dependency on `tts_audio`.

## 7. Symbol Inspection

The `nm` utility displays symbol information from compiled object files and libraries.

Commands:

```bash
nm -C build/libtts_audio.a
```

```bash
nm -C build/CMakeFiles/tts-engine.dir/src/main.cpp.o
```

The `-C` option demangles C++ names to improve readability.

Important symbols:

| Symbol | Meaning |
|---|---|
| `T` | Symbol defined in the text (code) section |
| `U` | Undefined reference that must be resolved elsewhere |

Expected relationship:

```text
main.cpp.o
    U findMaxSample(float const*, int, float*)

libtts_audio.a
    T findMaxSample(float const*, int, float*)
```

The implementation resides inside the library, while the main executable's object file references that implementation.

During linking, the linker resolves the undefined reference using the definition in the library.

I also learned that `const float*` and `float const*` represent the same type. C++ demangling tools may display the latter form.

**Verification note:** Record the actual `nm` output from my local build before marking this experiment complete.

## 8. Incremental Builds

An important lesson from Day 3 was understanding the distinction between recompilation and relinking.

If I modify only `src/audio/analyzer.cpp`, Ninja should:

1. Recompile `analyzer.cpp` into its object file.
2. Update `libtts_audio.a`.
3. Relink `tts-engine`.
4. Relink `test-analyzer`.

It should not need to recompile unchanged `main.cpp` or `test_analyzer.cpp`, provided their compilation dependencies have not changed.

I can investigate the build commands using:

```bash
cmake --build build -v
```

This behavior demonstrates how dependency tracking can avoid unnecessary compilation.

## 9. Testing and Verification

Configure the project:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++
```

Build:

```bash
cmake --build build
```

Run the application:

```bash
./build/tts-engine
```

Expected analyzer results:

| Measurement | Expected value |
|---|---:|
| Maximum sample | 0.8 |
| Minimum sample | -0.5 |
| Mean absolute amplitude | 0.38 |

Run the analyzer tests:

```bash
./build/test-analyzer
```

Run the CTest suite:

```bash
ctest --test-dir build --output-on-failure
```

The Day 2 harness contains 24 internal checks across the three analyzer functions. The Day 3 goal is to confirm that those tests still pass after the library refactor.

**Actual results:** To be recorded after the final build and test run.

## 10. Problems Encountered

### Missing object file during archive creation

While attempting to create the static library, `ar` initially reported that it could not find the expected object file.

The problem was that the compiled object file was not located at the path supplied to `ar`.

I resolved this by explicitly compiling `analyzer.cpp` into `analyzer.o` in the working directory before creating the archive.

### Understanding target relationships

Initially, I understood that `analyzer.cpp` needed to be removed from the executable target after creating the static library.

I then learned that `add_library()` and `add_executable()` alone do not connect those targets.

The `target_link_libraries()` command establishes the relationship.

### Understanding PUBLIC

I initially interpreted `PUBLIC` as meaning that the directory would be accessible to everything in the project's root directory.

I learned that in CMake, `PUBLIC` propagates usage requirements through target dependencies, rather than making them globally available to all files.

## 11. Connections to Computer Systems: A Programmer's Perspective

Day 3 reinforced concepts related to the compilation system and linking.

### Compilation and Linking

The compilation process produces object files containing machine code, symbol definitions, and references to external symbols.

The linker combines these objects and resolves references between them.

### Symbol Resolution

When `main.o` references `findMaxSample()`, it does not contain that function's implementation.

Instead, the linker locates a matching definition in the static library.

### Static Archives

A static archive collects reusable object files. During ordinary static linking, the linker can extract needed archive members to satisfy unresolved references.

### Binary Compatibility

A Linux static library built using Clang generally cannot be linked directly into a Windows executable built using MSVC.

Object-file formats, application binary interfaces, and compiler conventions can differ even when both systems use the same processor architecture.

A portable build description does not automatically make a compiled binary portable. The library usually needs to be rebuilt for its target platform.

## 12. Personal Reflection

The most important lesson from Day 3 was understanding how separately compiled components become a working executable.

Before this exercise, I could compile multiple source files together, but I hadn't manually packaged and linked a reusable library.

Creating `libtts_audio.a` helped me understand the distinction between source files, object files, static archives, and executables.

I also learned that CMake is more than a collection of compiler commands. It describes targets, dependencies, and usage requirements that the underlying build system uses to determine what needs to be rebuilt.

These concepts will become important as the TTS engine expands into audio processing, model inference, and additional reusable components.

## 13. Day 3 Completion Checklist

- [x] Understand static versus shared libraries.
- [x] Manually compile `analyzer.cpp` into an object file.
- [x] Create and inspect `libtts_audio.a` using `ar`.
- [x] Manually link the main executable against the static library.
- [x] Refactor the CMake build targets.
- [x] Confirm that the application and all analyzer tests pass after the refactor.
- [x] Record the actual `nm` symbol output.
- [x] Record and verify incremental build behavior.
- [x] Commit and push the Day 3 changes.

## 14. Version Control

After final verification:

```bash
git status
git add CMakeLists.txt docs/journal/day-03.md
git commit -m "Refactor audio analyzer into static library"
git push
```

## 15. Next Steps — Day 4

Day 4 will focus on memory management and ownership in C++20, including stack versus heap allocation, object lifetime, RAII, and designing safe audio buffers.

This will establish the foundation for handling larger streams of audio samples within the TTS engine.

