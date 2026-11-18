#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <algorithm>


int main() {
    std::vector<float> matW_1{1, 2, 3, -1, 0, 2};
    std::vector<float> x{2, 1, -1};
    //std::vector<float> x{-0.5, 0, 0};
    std::vector<float> h(2);
    std::vector<float> b{0.5, 1.5};

    std::size_t rows{2};
    std::size_t cols{3};

    for (std::size_t row{0}; row < rows; row++) {
        float sum{0};
        for(std::size_t col{0}; col < cols; col++) {
            sum += matW_1[row * cols + col] * x[col];

        }
        h[row] = std::max(static_cast<float>(0.0), (sum + b[row]));
        // ReLU application

        std::cout << "The output for h = ReLU(W_1x + b_1) is: " << h[row] << '\n';
    }

    float y{0};
    std::vector<float> matW_2{2,-1};
    float b_2{0.25};
    y = matW_2[0] * h[0] + matW_2[1] * h[1] + b_2;
    std::cout << "The output for y = W_2*h + b_2 is: " << y << '\n';

    return 0;
}
