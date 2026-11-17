#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdint>


int main() {
    std::vector<float> matW{1, 2, 3, -1, 0, 2};
   // std::vector<float> x{2, 1, -1};
    std::vector<float> x{0, 0, 0};
    std::vector<float> y(2);
    std::vector<float> b{0.5, 1.5};

    std::size_t rows{2};
    std::size_t cols{3};

    for (std::size_t row{0}; row < rows; row++) {
        int sum{0};
        for(std::size_t col{0}; col < cols; col++) {
            sum += matW[row * cols + col] * x[col];

        }
        y[row] = sum + b[row];
        std::cout << "The output for y = Wx + b is: " << y[row] << '\n';
    }
    
    return 0;
}
