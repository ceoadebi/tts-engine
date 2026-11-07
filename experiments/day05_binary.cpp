#include <cstdint>
#include <fstream>
#include <iostream>

namespace {

void writeUint32LE(std::ofstream& file, std::uint32_t value) {
    for (int i = 0; i < 4; ++i) {
        const std::uint8_t byte =
            static_cast<std::uint8_t>(
                (value >> (8 * i)) & 0xFFu
            );

        file.write(
            reinterpret_cast<const char*>(&byte),
            sizeof(byte)
        );
    }
}

} // namespace

int main() {
    std::ofstream file{"output.bin", std::ios::binary};

    if (!file) {
        std::cerr << "Failed to open output.bin\n";
        return 1;
    }

    writeUint32LE(file, 48000);

    file.close();

    if (!file) {
        std::cerr << "Failed to write output.bin\n";
        return 1;
    }

    std::cout << "Binary file written successfully\n";
    return 0;
}

