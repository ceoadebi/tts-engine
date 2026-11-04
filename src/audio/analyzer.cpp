// src/audio/analyzer.cpp
#include <cmath>
#include <algorithm>
#include <iostream>
#include "tts/audio/analyzer.hpp"


bool findMaxSample(const float* samples, int count, float* result) {
    if (count <= 0 || samples == nullptr || result == nullptr) {
        return false;
    }

    *result = samples[0];
    for (int i = 0; i < count; i++) {
        if (samples[i] > (*result)) {
            *result = samples[i];
        }
    }

    return true;
}

bool findMinSample(const float* samples, int count, float* result) {
    if (count <= 0 || samples == nullptr || result == nullptr) {
        return false;
    }

    *result = samples[0];
    for (int i = 0; i < count; i++) {
        if (samples[i] < (*result)) {
            *result = samples[i];
        }
    }

    return true;
}

bool calculateMeanAmplitude(const float* samples, int count, float* result) {
    if (count <= 0 || samples == nullptr || result == nullptr) {
        return false;
    }
    float sum = 0;
    for (int i = 0; i < count; i++) {
        sum += std::abs(samples[i]);
    }

    *result = sum / count;
    return true;
}
