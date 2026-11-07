#include <iostream>
/**
* Objective: 
* 1. Ask the user how many audio samples to allocate.
* 2. Allocate an array of float using the new[].
* 3. Initialize every sample to 0.0f.
* 4. Print the values.
* 5. Release the allocation using delete[].
*/

int main() {
    std::cout << "How many audio samples do you want to allocate? ";
    int sampleSize{};
    if (!(std::cin >> sampleSize) || sampleSize <= 0) {
        std::cerr << "Invalid sample count.\n";
        return 1;
    }

    float* array{new float[sampleSize]{} };
    
    for (int i{0}; i < sampleSize; i++) {
        std::cout << i << ": " << array[i] << '\n';
    }
    delete[] array;

    return 0;
}
