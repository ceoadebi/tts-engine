// tests/test_audio_buffer.cpp
#include <cstddef>
#include <iostream>
#include <utility>
#include "tts/audio/audio_buffer.hpp"
int main() {
    // ----------------------------------------
    // Test 1: Buffer Size
    // ----------------------------------------

    AudioBuffer buffer{100};

    if (buffer.size() != 100) {
        std::cerr << "Size test failed\n";
        return 1;
    }

    std::cout << "Size test passed\n";

    // ----------------------------------------
    // Test 2: Zero Initialization
    // ----------------------------------------

    for (std::size_t i = 0; i < buffer.size(); ++i) {
        if (buffer.data()[i] != 0.0f) {
            std::cerr << "Data test failed at index "
                      << i << '\n';
            return 1;
        }
    }

    std::cout << "Data test passed\n";

    // ----------------------------------------
    // Test 3: Read/Write
    // ----------------------------------------

    buffer.data()[25] = 0.75f;

    if (buffer.data()[25] != 0.75f) {
        std::cerr << "Write test failed\n";
        return 1;
    }

    std::cout << "Write test passed\n";

    // ----------------------------------------
    // Test 4: Move Constructor
    // ----------------------------------------

    AudioBuffer original{100};
    original.data()[0] = 0.75f;

    AudioBuffer moved{std::move(original)};

    // Verify the source buffer is empty
    if (original.size() != 0) {
        std::cerr << "Move test failed: source size\n";
        return 1;
    }

    if (original.data() != nullptr) {
        std::cerr << "Move test failed: source pointer\n";
        return 1;
    }

    // Verify the destination owns the samples
    if (moved.size() != 100) {
        std::cerr << "Move test failed: destination size\n";
        return 1;
    }

    if (moved.data() == nullptr) {
        std::cerr << "Move test failed: destination pointer\n";
        return 1;
    }

    if (moved.data()[0] != 0.75f) {
        std::cerr << "Move test failed: sample value\n";
        return 1;
    }

    std::cout << "Move test passed\n";

    std::cout << "\nAll AudioBuffer tests passed!\n";


    // ----------------------------------------
    // Test 5: Move Assignment
    // ----------------------------------------
    AudioBuffer first{100};
    AudioBuffer second{200};
    
    first.data()[0] = 0.75f;

    second = std::move(first);

    if (first.size() != 0) {
        std::cerr << "Move assignment failed: source size\n";
        return 1;
    }

    if (first.data() != nullptr) {
        std::cerr << "Move assignment failed: source pointer\n";
        return 1;
    }

    if (second.size() != 100) {
        std::cerr << "Move assignment failed: destination size\n";
        return 1;
    }

    if (second.data() == nullptr ||
        second.data()[0] != 0.75f) {
        std::cerr << "Move assignment failed: sample value\n";
        return 1;
    }

    std::cout << "Move assignment test passed\n";

    // ----------------------------------------
    // Test 6: Self-Move Assignment
    // ----------------------------------------

    AudioBuffer self{100};
    self.data()[0] = 0.5f;

    // Perform an intentional self-move test
    AudioBuffer& alias = self;
    self = std::move(alias);

    if (self.size() != 100) {
        std::cerr << "Self-move test failed: size\n";
        return 1;
    }

    if (self.data() == nullptr ||
        self.data()[0] != 0.5f) {
        std::cerr << "Self-move test failed: sample value\n";
        return 1;
    }

    std::cout << "Self-move assignment test passed\n";

    // ----------------------------------------
    // All Tests Passed
    // ----------------------------------------

    std::cout << "\nAll AudioBuffer tests passed!\n";

    return 0;
}

