// include/tts/audio/analyzer.hpp
#ifndef TTS_AUDIO_ANALYZER_HPP
#define TTS_AUDIO_ANALYZER_HPP

/**
* Takes 3 inputs: pointer to audio samples, count, pointer to result
* Returns largest sample value
*/
bool findMaxSample(const float* samples, int count, float* result);

/**
* Takes 3 inputs: pointer to audio samples, count, pointer to result
* Returns smallest sample value 
*/
bool findMinSample(const float* samples, int count, float* result);

/**
* Takes 3 inputs: pointer to audio samples, count, pointer to result
* Returns mean absolute amplitude 
*/
bool calculateMeanAmplitude(const float* samples, int count, float* result);

/**
* Takes 3 inputs: pointer to audio samples, count, pointer to result
* Returns root mean squared amplitude 
*/
bool calculateRMS(const float* samples, int count, float* result);

#endif 

