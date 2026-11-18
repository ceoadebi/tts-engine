#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <algorithm>


int main() {
    std::vector<float> matW{1, 2, 3, -1, 0, 2};
    //std::vector<float> x{2, 1, -1};
    std::vector<float> x{-0.5, 0, 0};
    std::vector<float> y(2);
    std::vector<float> b{0.5, 1.5};

    std::size_t rows{2};
    std::size_t cols{3};

    for (std::size_t row{0}; row < rows; row++) {
        float sum{0};
        for(std::size_t col{0}; col < cols; col++) {
            sum += matW[row * cols + col] * x[col];

        }
        y[row] = std::max(static_cast<float>(0.0), (sum + b[row]));
        // ReLU application

        std::cout << "The output for y = Wx + b is: " << y[row] << '\n';
    }

    return 0;
}
