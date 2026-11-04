// src/main.cpp
#include <iostream>
#include "tts/audio/analyzer.hpp"

int main() {
    float samples[] = {0.2f, -0.5f, 0.8f, -0.3f, 0.1f};
    int sampleLength = std::size(samples);
    float maxSample = samples[0];
    if (!findMaxSample(samples, sampleLength, &maxSample)) {
        return 1;
    }

    float minSample = samples[0];
    if (!findMinSample(samples, sampleLength, &minSample)) {
        return 1;
    }

    float meanSample = 0;
    if (!calculateMeanAmplitude(samples, sampleLength, &meanSample)) {
        return 1;
    }


    std::cout << "TTS Engine initialized!\n";
    std::cout << "Audio Sampler Analyzer\n";
    std::cout << "----------------------\n";
    std::cout << "Maximum: " << maxSample << "\n";
    std::cout << "Minimum: " << minSample << "\n";
    std::cout << "Mean: " << meanSample << "\n";

    std::cout << "========TESTS========\n";
    if (!calculateMeanAmplitude(samples, 0, &meanSample)) {
        std::cout << "The test for calculateaMeanAmplitude has failed!\n";     
    }

    return 0;
}
