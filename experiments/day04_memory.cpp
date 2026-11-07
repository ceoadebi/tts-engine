#include <iostream>

int main() {
    float sample = 0.5f;

    float samples[5] = {0.2f, -0.5f, 0.8f, -0.3f, 0.1f};

    float* dynamicSamples = new float[5];

    dynamicSamples[0] = 0.5f;

    delete[] dynamicSamples;
    
    return 0;
}
