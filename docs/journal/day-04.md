# Day 4 — Memory Management, RAII, and AudioBuffer Design

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 5, 2026  
**Environment:** Ubuntu (WSL2), Clang, CMake, Ninja  
**Focus:** Dynamic memory allocation, object lifetime, ownership, RAII, move semantics, and resource management  
**Status:** Core implementation and functional testing completed

---

## 1. Objective

The objective of Day 4 was to understand memory management and resource ownership in C++20 and apply these concepts to the development of a reusable audio buffer.

During Day 3, I learned how compiled object files are packaged into static libraries and linked into executables. Day 4 shifted the focus from compilation and linking to runtime memory management.

The primary goal was to develop an `AudioBuffer` class that dynamically allocates audio samples, manages their lifetime automatically, and supports efficient ownership transfers without copying the underlying sample data.

Specific objectives included:

- Understand stack and heap memory.
- Investigate automatic and dynamic storage duration.
- Understand object lifetime, constructors, and destructors.
- Allocate and release dynamic arrays using `new[]` and `delete[]`.
- Identify memory leaks, dangling pointers, and double-free errors.
- Learn Resource Acquisition Is Initialization (RAII).
- Understand `std::unique_ptr` and exclusive ownership.
- Implement move construction and move assignment.
- Explore const-correctness and exception specifications.
- Integrate the `AudioBuffer` implementation into the existing static library.
- Develop tests to verify allocation, initialization, read/write operations, and ownership transfers.

## 2. Development Environment

| Component | Tool |
|---|---|
| Operating system | Ubuntu through WSL2 |
| Programming language | C++20 |
| Compiler | Clang |
| Build configuration | CMake |
| Build system | Ninja |
| Editor | Neovim |
| Testing framework | Custom C++ test harness and CTest |
| Version control | Git and GitHub |
| Debugging tools | AddressSanitizer and UndefinedBehaviorSanitizer (available for experiments) |

## 3. Project Structure

The project was expanded with a reusable audio buffer implementation and its associated tests.

```text
tts-engine/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── .gitignore
├── include/
│   └── tts/
│       └── audio/
│           ├── analyzer.hpp
│           └── audio_buffer.hpp
├── src/
│   ├── main.cpp
│   └── audio/
│       ├── analyzer.cpp
│       └── audio_buffer.cpp
├── tests/
│   ├── test_analyzer.cpp
│   └── test_audio_buffer.cpp
├── experiments/
│   ├── day04_memory.cpp
│   └── day04_dynamic.cpp
└── docs/
    ├── setup.md
    └── journal/
        ├── day-01.md
        ├── day-02.md
        ├── day-03.md
        └── day-04.md
```

The new `AudioBuffer` component follows the separation between public interface and implementation established earlier in the project.

- `audio_buffer.hpp` declares the class.
- `audio_buffer.cpp` defines its member functions.
- `test_audio_buffer.cpp` verifies its behavior.

The experimental files are separate from the production library. Their presence in the repository should be confirmed before committing.

## 4. Stack vs. Heap Memory

The first topic explored was the difference between automatic and dynamically allocated storage.

Consider:

```cpp
int main() {
    float sample = 0.5f;

    float samples[5]{
        0.2f, -0.5f, 0.8f, -0.3f, 0.1f
    };

    float* dynamicSamples = new float[5]{};

    delete[] dynamicSamples;

    return 0;
}
```

The variables `sample`, `samples`, and `dynamicSamples` have automatic storage duration and are typically stored on the stack.

However, the array created using `new[]` has dynamic storage duration and is typically allocated on the heap.

The pointer variable and the allocation it references are separate entities.

### Automatic storage

Objects with automatic storage duration are destroyed when their lifetimes end, normally upon exiting their scope.

For simple local variables such as `float`, no explicit cleanup is necessary.

### Dynamic storage

An allocation created using `new[]` persists until it is explicitly deallocated or its lifetime is managed by an owning object.

Allowing a raw pointer to go out of scope does not automatically delete the allocation it references.

### Key lesson

The lifetime of a pointer variable is not necessarily the lifetime of the memory to which it points.

This distinction is fundamental to resource management.

## 5. Object Lifetime and Destructors

The next experiment explored how C++ automatically invokes destructors when objects leave scope.

Example:

```cpp
#include <iostream>

struct AudioTracker {
    AudioTracker() {
        std::cout << "Constructor called\n";
    }

    ~AudioTracker() {
        std::cout << "Destructor called\n";
    }
};

int main() {
    std::cout << "Program started\n";

    {
        AudioTracker tracker;
        std::cout << "Inside scope\n";
    }

    std::cout << "Outside scope\n";

    return 0;
}
```

Expected output:

```text
Program started
Constructor called
Inside scope
Destructor called
Outside scope
```

The important observation is that the destructor runs when the object leaves its inner scope, rather than waiting until `main()` returns.

This illustrates how C++ associates cleanup behavior with object lifetime.

It also provides the foundation for RAII.

## 6. Dynamic Array Allocation

I investigated the relationship between C-style manual allocation and C++ dynamic allocation.

In C:

```c
float* samples = malloc(100 * sizeof(float));
free(samples);
```

In C++:

```cpp
float* samples = new float[100]{};
delete[] samples;
```

Both approaches require the programmer to manage the allocation's lifetime explicitly when using raw owning pointers.

### Initialization

I learned the difference between:

```cpp
float* samples = new float[100];
```

and:

```cpp
float* samples = new float[100]{};
```

The first form default-initializes the elements, leaving the fundamental `float` values indeterminate.

The second form value-initializes the elements, producing `0.0f` for every sample.

This is useful for audio processing because a zero-valued floating-point PCM sample conventionally represents silence.

### Dynamic allocation experiment

I implemented a program that accepted a requested sample count, dynamically allocated an array, initialized its values to zero, printed the elements, and released the allocation.

The allocation used brace initialization:

```cpp
float* array{new float[sampleSize]{}};
```

The terminal output confirmed that the tested sample values were initialized to zero.

### Input validation

During the experiment, I investigated the problem of accepting negative values into an unsigned size type.

I learned that `std::size_t` cannot represent negative values and that untrusted input should be validated before conversion and allocation.

A suitable validation approach is:

```cpp
int sampleSize{};

if (!(std::cin >> sampleSize) || sampleSize <= 0) {
    std::cerr << "Invalid sample count.\n";
    return 1;
}
```

Once validation succeeds, the count can be converted to an appropriate allocation size type.

I also learned that `assert()` should not replace input validation because assertions can be disabled and are primarily intended for checking programming assumptions.

## 7. Memory Ownership Problems

Before introducing RAII, I examined three common memory-management errors.

### 7.1 Memory leaks

```cpp
float* samples = new float[100]{};

samples = new float[200]{};

delete[] samples;
```

The second allocation overwrites the only pointer to the first allocation.

Consequently, the original 100-element allocation becomes unreachable and cannot be released through that pointer.

This causes a memory leak.

### 7.2 Dangling pointers

```cpp
float* samples = new float[100]{};
float* other = samples;

delete[] samples;
```

After the deletion, both pointers retain invalid addresses.

Accessing the previously allocated elements through either pointer causes undefined behavior.

Setting a pointer to `nullptr` can prevent later accidental dereferencing through that particular pointer, but it does not automatically update other pointers referring to the same allocation.

### 7.3 Double-free errors

```cpp
float* samples = new float[100]{};
float* other = samples;

delete[] samples;
delete[] other;
```

The second deallocation attempts to release an allocation that has already been freed.

This is undefined behavior.

### Key lesson

An allocation should have a clearly defined owner responsible for releasing it.

Multiple pointers may access an allocation, but access does not imply ownership.

## 8. RAII and std::unique_ptr

Resource Acquisition Is Initialization (RAII) is a C++ resource-management technique that associates resource ownership with an object's lifetime.

Instead of manually releasing memory, an owning object's destructor performs cleanup automatically.

Example:

```cpp
#include <memory>

int main() {
    std::unique_ptr<float[]> samples{
        new float[100]{}
    };

    samples[0] = 0.5f;

    return 0;
}
```

When `samples` leaves scope, its destructor automatically releases the dynamically allocated array.

The major benefits include:

- Automatic cleanup during normal scope exit.
- Automatic cleanup during stack unwinding from exceptions.
- Clear ownership semantics.
- Reduced risk of memory leaks.
- Prevention of accidental copying of exclusive ownership.

### Access versus ownership

The `.get()` member function returns the underlying raw pointer without transferring ownership.

The `.release()` member function relinquishes ownership and returns the raw pointer, leaving the caller responsible for cleanup.

This distinction became important when designing the `AudioBuffer` interface.

## 9. Designing AudioBuffer

I created an `AudioBuffer` class to encapsulate ownership of dynamically allocated floating-point audio samples.

The class stores:

```cpp
std::unique_ptr<float[]> m_data;
std::size_t m_size;
```

The smart pointer owns the sample allocation, while `m_size` records the number of samples.

The public interface includes a constructor, move constructor, move-assignment operator, a data accessor, and a size accessor.

### 9.1 Header file

File: `include/tts/audio/audio_buffer.hpp`

```cpp
#ifndef TTS_AUDIO_AUDIO_BUFFER_HPP
#define TTS_AUDIO_AUDIO_BUFFER_HPP

#include <cstddef>
#include <memory>

class AudioBuffer {
private:
    std::unique_ptr<float[]> m_data;
    std::size_t m_size;

public:
    explicit AudioBuffer(std::size_t count);

    AudioBuffer(AudioBuffer&& other) noexcept;
    AudioBuffer& operator=(AudioBuffer&& other) noexcept;

    float* data();
    std::size_t size() const;
};

#endif // TTS_AUDIO_AUDIO_BUFFER_HPP
```

The header guard prevents repeated processing of the class definition within the same translation unit.

The header includes `<memory>` and `<cstddef>` because it directly depends on `std::unique_ptr` and `std::size_t`.

### 9.2 Constructor implementation

File: `src/audio/audio_buffer.cpp`

```cpp
#include "tts/audio/audio_buffer.hpp"

#include <utility>

AudioBuffer::AudioBuffer(std::size_t count)
    : m_data{new float[count]{}},
      m_size{count}
{
}
```

The member initializer list directly initializes the object's members.

`m_data` owns an array containing `count` zero-initialized floats.

The members are initialized in declaration order, regardless of their textual order in the initializer list.

### 9.3 Data accessor

```cpp
float* AudioBuffer::data() {
    return m_data.get();
}
```

This function provides non-owning access to the audio samples.

It does not transfer ownership.

### 9.4 Size accessor

```cpp
std::size_t AudioBuffer::size() const {
    return m_size;
}
```

The trailing `const` prevents the member function from ordinarily modifying the object's non-mutable data members.

I also investigated why it is common for C++ interfaces to provide both mutable and const overloads of data accessors.

A future improvement would be:

```cpp
float* data();
const float* data() const;
```

This would allow read-only access through const-qualified buffer objects.

## 10. Move Semantics

Because `AudioBuffer` contains a `std::unique_ptr`, the class cannot be copied using its default copy operations.

Copying a unique owner would create ambiguity about which object is responsible for releasing the allocation.

Instead, ownership can be transferred using move semantics.

### 10.1 Move constructor

The move constructor accepts an rvalue reference:

```cpp
AudioBuffer(AudioBuffer&& other) noexcept;
```

Implementation:

```cpp
AudioBuffer::AudioBuffer(AudioBuffer&& other) noexcept
    : m_data{std::move(other.m_data)},
      m_size{other.m_size}
{
    other.m_size = 0;
}
```

During the move:

1. Ownership of the sample array transfers into the destination's `m_data`.
2. The sample count is copied into the destination.
3. The source object's `m_size` is reset to zero.

This leaves the moved-from object in a consistent empty state.

### 10.2 Why reset the source size?

A compiler-generated move constructor would move the smart pointer but copy the integer size.

This could leave the moved-from object in a state where:

```text
m_data = nullptr
m_size = 100
```

If another function assumed that a nonzero size guaranteed valid sample storage, it might attempt to dereference a null pointer.

The custom move constructor maintains the invariant that a positive sample count implies valid sample storage.

### 10.3 Move assignment

Move assignment differs from move construction because the destination object already exists and may own resources.

The move-assignment operator was declared as:

```cpp
AudioBuffer& operator=(AudioBuffer&& other) noexcept;
```

Implementation:

```cpp
AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) noexcept {
    if (this != &other) {
        m_data = std::move(other.m_data);
        m_size = other.m_size;
        other.m_size = 0;
    }

    return *this;
}
```

When the destination's `m_data` receives ownership from the source, its previous allocation is automatically released.

The `this != &other` comparison protects against self-move assignment.

Returning `*this` provides a reference to the destination object.

### 10.4 The role of std::move

`std::move()` does not physically move memory or copy the sample array.

It casts its argument to an rvalue expression, allowing move operations to be selected.

For `std::unique_ptr`, the move operation transfers ownership of the underlying pointer.

The allocated samples remain at their original memory address.

## 11. noexcept and Exception Safety

I learned that `noexcept` specifies that a function will not allow exceptions to propagate out of it.

The move constructor and move-assignment operator were marked `noexcept` because their operations involve non-throwing smart-pointer ownership transfers and integer assignments.

```cpp
AudioBuffer(AudioBuffer&& other) noexcept;
AudioBuffer& operator=(AudioBuffer&& other) noexcept;
```

The allocation constructor was not marked `noexcept`:

```cpp
explicit AudioBuffer(std::size_t count);
```

This is because memory allocation using `new[]` can fail and throw an exception such as `std::bad_alloc`.

Declaring an allocating constructor `noexcept` would cause termination if an exception escaped.

Non-throwing moves are also useful for interoperability with standard-library containers, which can take advantage of move operations when reorganizing stored objects.

## 12. CMake Integration

The `AudioBuffer` implementation was added to the existing `tts_audio` static library.

Updated target:

```cmake
add_library(tts_audio STATIC
    src/audio/analyzer.cpp
    src/audio/audio_buffer.cpp
)
```

The public include directory remained associated with the library:

```cmake
target_include_directories(tts_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

This allows targets linking to `tts_audio` to access the public audio headers.

The new test target was registered using:

```cmake
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
```

The existing analyzer tests remained part of the project.

## 13. AudioBuffer Testing

A dedicated test executable was created to verify the new audio buffer.

File: `tests/test_audio_buffer.cpp`

### Test 1: Buffer size

Construct an `AudioBuffer` containing 100 samples.

Verify that:

```cpp
buffer.size() == 100
```

### Test 2: Zero initialization

Iterate through every sample and verify that its initial value is `0.0f`.

The test exits immediately upon discovering an incorrect value and reports success only after the entire array has been checked.

### Test 3: Read/write

Assign:

```cpp
buffer.data()[25] = 0.75f;
```

Verify that the value can be retrieved from the same location.

### Test 4: Move construction

Create a 100-sample buffer, assign `0.75f` to its first sample, and move it into another object.

Verify that:

| Expression | Expected |
|---|---|
| Source `size()` | `0` |
| Source `data()` | `nullptr` |
| Destination `size()` | `100` |
| Destination first sample | `0.75f` |

### Test 5: Move assignment

Construct two buffers containing 100 and 200 samples.

Move the first buffer into the second.

Verify that the destination now owns 100 samples and that the source is empty.

Also verify that the original sample value remains unchanged.

### Test 6: Self-move assignment

Perform self-move assignment through an alias:

```cpp
AudioBuffer self{100};
self.data()[0] = 0.5f;

AudioBuffer& alias = self;
self = std::move(alias);
```

Verify that the size remains 100 and the first sample remains `0.5f`.

This confirms that the self-assignment guard preserves the object.

### Test results

The completed test suite was built and run successfully.

Expected successful output:

```text
Size test passed
Data test passed
Write test passed
Move constructor test passed
Move assignment test passed
Self-move assignment test passed

All AudioBuffer tests passed!
```

The user-reported results confirmed that the functional tests passed after the `noexcept` changes.

## 14. Build and Verification Commands

Configure the project with Clang and Ninja:

```bash
cmake -S . -B build -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++
```

Compile the project:

```bash
cmake --build build
```

Run the audio buffer tests:

```bash
./build/test-audio-buffer
```

Run all CTest tests:

```bash
ctest --test-dir build --output-on-failure
```

With the analyzer and audio buffer test executables both registered and passing, CTest should report:

```text
100% tests passed, 0 tests failed out of 2
```

### Additional memory-safety verification

An optional follow-up is to build the project with AddressSanitizer and UndefinedBehaviorSanitizer enabled.

Use a separate build directory:

```bash
cmake -S . -B build-sanitize -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

cmake --build build-sanitize

ctest --test-dir build-sanitize --output-on-failure
```

The sanitizer run has not yet been independently recorded as completed.

## 15. Problems Encountered and Lessons Learned

### 15.1 Using assert for input validation

I initially considered using `assert()` to ensure that a sample count was positive.

I learned that assertions are better suited to internal programming assumptions, while ordinary conditionals should validate external input.

### 15.2 Confusing pointer lifetime and allocation lifetime

I learned that a raw pointer leaving scope does not automatically release the allocation it references.

This motivated the transition toward RAII.

### 15.3 Confusing const member functions with const local variables

I investigated `size() const` and learned that the trailing `const` protects the object's state through that member function rather than making all its local variables immutable.

### 15.4 Attempting to assign through size()

While designing the move constructor, I initially attempted:

```cpp
other.size() = 0;
```

This is invalid because `size()` returns a value rather than a reference.

The correct approach inside the class implementation is:

```cpp
other.m_size = 0;
```

### 15.5 Confusing references and pointers

During move-assignment development, I considered accessing the source using `other->m_data`.

I learned that `other` is a reference, so its member should be accessed using the dot operator:

```cpp
other.m_data
```

### 15.6 Copying unique_ptr

I initially considered directly assigning one smart pointer to another.

I learned that copying a `std::unique_ptr` is prohibited.

Instead, ownership is transferred through move operations.

### 15.7 Returning the current object

The move-assignment operator returns `AudioBuffer&`.

I learned that `this` is a pointer, while `*this` denotes the current object and can be returned as a reference.

## 16. Connections to Computer Systems: A Programmer's Perspective

Day 4 reinforced several concepts from *Computer Systems: A Programmer's Perspective* (CS:APP).

### 16.1 Process virtual memory

A running program operates within a virtual address space containing regions for instructions, static data, dynamically allocated memory, and thread stacks.

Understanding these regions helps explain why local variables and dynamically allocated objects have different lifetimes.

### 16.2 Dynamic memory allocation

Heap allocation requires runtime memory management.

Incorrect allocation and deallocation behavior can produce leaks, invalid memory accesses, and allocator corruption.

### 16.3 Memory safety

Dangling pointers and double-free errors illustrate the dangers of accessing or releasing memory after its lifetime has ended.

C++ RAII reduces the need for manual deallocation and helps prevent these classes of errors.

### 16.4 Pointers and indirection

The `AudioBuffer` interface separates the object that owns an allocation from raw pointers that access it.

This reinforces the distinction between pointer values, addresses, and the underlying objects.

### 16.5 Program efficiency

Move semantics allow an existing allocation to change owners without copying every element.

For large audio buffers, this can avoid expensive memory transfers.

### 16.6 C++ abstractions over systems concepts

RAII and smart pointers illustrate how language-level abstractions can provide safer interfaces while retaining direct control over dynamically allocated memory.

## 17. Personal Reflection

The most important concept I learned on Day 4 was that memory ownership should be explicit.

Coming from C, I was familiar with `malloc()`, `free()`, raw pointers, and manually managed arrays. C++ provides additional mechanisms that allow resources to be associated with object lifetimes.

Implementing `AudioBuffer` helped me understand why RAII is valuable beyond simply reducing the number of `delete[]` statements.

I also learned that moving a resource differs fundamentally from copying it. The move constructor and move-assignment operator transfer ownership of the existing allocation rather than duplicating the underlying audio samples.

Another important lesson was the need to maintain class invariants. A moved-from buffer should not report that it owns samples when its data pointer is null.

These concepts will become increasingly important as the TTS engine begins processing larger audio streams and integrates model inference.

## 18. Day 4 Completion Checklist

- [x] Understand stack versus heap allocation.
- [x] Investigate object lifetime and destructors.
- [x] Implement a dynamically allocated floating-point array.
- [x] Understand memory leaks, dangling pointers, and double-free errors.
- [x] Learn RAII and `std::unique_ptr`.
- [x] Implement the `AudioBuffer` constructor.
- [x] Implement the `data()` and `size()` accessors.
- [x] Implement the move constructor.
- [x] Implement the move-assignment operator.
- [x] Add self-move protection.
- [x] Mark move operations `noexcept`.
- [x] Integrate `AudioBuffer` into the `tts_audio` static library.
- [x] Create and run functional tests successfully.
- [x] Run AddressSanitizer and UndefinedBehaviorSanitizer verification.
- [x] Commit and push the completed Day 4 changes.

## 19. Version Control

After completing the journal, inspect the working tree:

```bash
git status
```

Stage the source, header, tests, build configuration, and journal:

```bash
git add CMakeLists.txt \
    include/tts/audio/audio_buffer.hpp \
    src/audio/audio_buffer.cpp \
    tests/test_audio_buffer.cpp \
    docs/journal/day-04.md
```

If the experimental files are also intended to be version-controlled, stage those separately.

Commit:

```bash
git commit -m "Implement RAII audio buffer with move semantics"
```

Push:

```bash
git push
```

## 20. Next Steps — Day 5

Day 5 will focus on audio sample representation and the fundamentals of digital audio.

Topics will include:

- Digital sampling and sample rates.
- Amplitude and normalized floating-point samples.
- PCM encoding and quantization.
- Integer versus floating-point sample formats.
- Bit depth and dynamic range.
- Mono and stereo channel layouts.
- WAV file structure and RIFF headers.
- Preparing `AudioBuffer` for audio file input and output.

The next major milestone is to understand how raw sample data is represented in memory and stored in an audio file.

This will provide the foundation for generating and processing real audio using the TTS engine.

