# Day 9 — Audio Peak Normalization and Clipping Management

**Project:** C++20 Text-to-Speech Engine  
**Development Environment:** Ubuntu (WSL2), Clang, CMake, Ninja, Neovim  
**Status:** Implementation and testing complete

## 1. Objectives

The primary objective of Day 9 was to implement peak normalization and improve amplitude management within my C++20 text-to-speech audio engine.

During Day 8, I implemented audio mixing with independent gain controls. While the mixer successfully combined signals, mixing could produce amplitudes exceeding the normalized audio range of `[-1.0, 1.0]`.

Day 9 focused on managing these amplitudes before PCM16 conversion to help prevent hard clipping.

The main objectives were:

- Understand the difference between clipping and normalization.
- Measure the peak absolute amplitude of an audio signal.
- Implement a reusable peak-normalization component.
- Support amplification and attenuation of audio signals.
- Validate normalization targets and audio samples.
- Handle silence and empty buffers safely.
- Implement automated tests for normalization behavior.
- Integrate normalization with the existing audio-processing pipeline.
- Generate and verify a normalized PCM16 WAV file.

## 2. Peak Normalization Fundamentals

Peak normalization scales an entire audio signal so that its maximum absolute amplitude reaches a specified target.

The first step is to determine the peak absolute amplitude:

\[
P = \max_n |x[n]|
\]

Where:

- \(x[n]\) represents an individual audio sample.
- \(P\) represents the largest absolute sample magnitude.

For example, consider the following samples:

```cpp
float samples[] = {
    0.4f, -0.8f, 1.4f, -1.2f, 0.6f
};
```

The peak absolute amplitude is `1.4`, because it is the largest magnitude in the buffer.

Once the peak is known, the normalization gain is calculated:

\[
g = \frac{P_{\text{target}}}{P}
\]

The gain is then applied uniformly to the entire signal:

\[
y[n] = g \times x[n]
\]

### Example Calculation

Given:

- Measured peak: `1.4`
- Target peak: `0.9`

The gain is:

\[
g = \frac{0.9}{1.4} \approx 0.642857
\]

Applying this gain reduces every sample proportionally.

Unlike hard clipping, normalization preserves the waveform's relative amplitudes and overall shape, subject to floating-point rounding.

## 3. Clipping Versus Normalization

Hard clipping occurs when a signal exceeds an allowed amplitude range and the values outside that range are limited to the maximum or minimum.

For normalized floating-point audio, the intended output range is generally `[-1.0, 1.0]`.

For example:

```cpp
float sample = 1.4f;
```

When passed to the PCM16 converter, this value is clamped to `1.0f`.

Hard clipping changes individual waveform peaks and can introduce harmonic distortion.

Peak normalization instead adjusts the gain of the entire signal before conversion.

This reduces the likelihood of clipping while preserving the relative differences between samples.

An important limitation is that normalization cannot restore waveform information that has already been lost through clipping.

Peak normalization also differs from compression and limiting because it applies one gain multiplier to the entire buffer rather than changing the gain dynamically over time.

## 4. Normalizer Implementation

I created the following files:

- `include/tts/audio/normalizer.hpp`
- `src/audio/normalizer.cpp`
- `tests/test_normalizer.cpp`

The public interface is:

```cpp
bool normalizePeak(
    AudioBuffer& buffer,
    float targetPeak
);
```

The function modifies an existing `AudioBuffer` in place and returns a boolean indicating whether processing succeeded.

### Input Validation

The implementation verifies that:

- The target peak is finite.
- The target peak is between `0.0f` and `1.0f`, inclusive.
- All input samples are finite.
- Invalid inputs are rejected before modifying the buffer.

This protects against NaN, positive infinity, negative infinity, and invalid normalization targets.

### Peak Detection

The normalizer scans every audio sample to determine the largest absolute amplitude.

The peak-detection logic uses:

```cpp
float peak{0.0f};

for (std::size_t i{0}; i < buffer.size(); ++i) {
    const float sample = buffer.data()[i];

    if (!std::isfinite(sample)) {
        return false;
    }

    peak = std::max(peak, std::abs(sample));
}
```

Using `std::abs()` ensures that negative sample values contribute correctly to the peak measurement.

### Silent Buffers

A silent buffer has a measured peak of zero.

Attempting to calculate `targetPeak / peak` would cause division by zero.

To handle this case, I added:

```cpp
if (peak == 0.0f) {
    return true;
}
```

Silence is considered valid audio, so the function succeeds without modifying the buffer.

This also handles an empty audio buffer.

### Applying Normalization

For a non-silent buffer, the gain is calculated as:

```cpp
const float gain = targetPeak / peak;
```

The gain is then applied to every sample:

```cpp
for (std::size_t i{0}; i < buffer.size(); ++i) {
    buffer.data()[i] *= gain;
}
```

The function supports both amplification and attenuation.

For example, a buffer with a peak of `0.4` and a target of `0.8` receives a gain multiplier of `2.0`.

A buffer with a peak of `1.4` and a target of `0.7` receives a gain multiplier of `0.5`.

## 5. C++20 Concepts Applied

### References

The normalizer accepts an `AudioBuffer&`, allowing it to modify the original audio data without copying the buffer.

### Standard Algorithms

I used `std::max()` from `<algorithm>` to calculate the maximum absolute sample magnitude.

### Mathematical Functions

I used functions from `<cmath>`, including:

- `std::abs()` for sample magnitude.
- `std::isfinite()` for floating-point validation.

### Floating-Point Arithmetic

The normalizer uses `float` values for amplitude measurements and gain application, matching the existing audio-buffer representation.

### `std::size_t`

Sample indices use `std::size_t`, consistent with the type returned by `AudioBuffer::size()`.

### Boolean Return Values

The function returns `false` for invalid inputs and `true` when normalization completes successfully or the buffer is already silent.

### In-Place Processing

Normalization modifies the existing audio buffer without allocating an additional buffer for the processed signal.

This design supports efficient processing and integrates naturally with the existing audio pipeline.

## 6. Automated Testing

I created:

`tests/test_normalizer.cpp`

The test executable was integrated into CMake and CTest.

### Tests Performed

The tests verified:

- Amplification of quiet signals.
- Attenuation of loud signals.
- Preservation of signals already at the target peak.
- Correct handling of silent buffers.
- Normalization to a zero target peak.
- Rejection of negative target amplitudes.
- Rejection of target amplitudes above `1.0`.
- Rejection of NaN and infinite target values.
- Rejection of NaN and infinite audio samples.
- Preservation of audio data following validation failures.
- Correct behavior with empty buffers.
- Absolute peak detection for negative samples.
- Correct final peak amplitude after normalization.
- Preservation of the original sample count.

### Test Results

The normalizer test suite passed successfully.

The tests confirmed that the normalizer scales audio samples as expected and handles the tested invalid-input conditions without modifying the original buffer.

### Debugging

During implementation review, I identified a missing check for silent buffers.

Initially, the function calculated the normalization gain without first checking whether the peak amplitude was zero.

I corrected the issue by adding an early return for silent audio.

After the correction, the automated tests passed.

## 7. Integration with the Audio Pipeline

I created the following experiment:

`experiments/day09_normalization.cpp`

The experiment extended the Day 8 mixing pipeline by introducing peak normalization before WAV export.

### Processing Pipeline

**Sine Generators → Audio Buffers → Mixer → Amplitude Envelope → Peak Normalizer → PCM16 Converter → WAV Writer**

Two sine waves were generated using the existing signal generator.

| Property | Signal A | Signal B |
|---|---|---|
| Frequency | 440 Hz | 660 Hz |
| Sample rate | 48,000 Hz | 48,000 Hz |
| Initial amplitude | 0.8 | 0.8 |
| Mixing gain | 0.5 | 0.5 |

The mixer combined the signals into a single output buffer.

The amplitude envelope then applied:

- Attack: 0.1 seconds
- Sustain: 0.8 seconds
- Release: 0.1 seconds

After envelope processing, the normalizer adjusted the signal's peak amplitude to a target of `0.9`.

### Output File

The experiment generated:

`mixed_440_660_normalized.wav`

The output properties were:

| Property | Value |
|---|---|
| Sample rate | 48,000 Hz |
| Duration | 1 second |
| Channels | 1 (mono) |
| Audio format | 16-bit PCM WAV |
| Audio samples | 48,000 |
| Audio data size | 96,000 bytes |
| WAV header | 44 bytes |
| Total file size | 96,044 bytes |
| Target normalized peak | 0.9 |

The WAV file was generated and played successfully.

## 8. Peak Amplitude Verification

After applying normalization, I measured the output peak independently within the experiment.

The measurement used the same absolute peak calculation:

```cpp
float measuredPeak{0.0f};

for (std::size_t i{0}; i < output.size(); ++i) {
    measuredPeak = std::max(
        measuredPeak,
        std::abs(output.data()[i])
    );
}
```

The measured peak was approximately:

**0.9**

This confirmed that the normalizer achieved its target amplitude before PCM16 conversion.

The WAV file was also played successfully, demonstrating that normalization integrated correctly with the rest of the audio engine.

## 9. Challenges and Lessons Learned

### Absolute Amplitude

I reinforced the importance of using absolute values when measuring peak amplitude.

Negative sample values can have larger magnitudes than positive samples and must be considered when determining the peak.

### Division by Zero

Silent buffers require explicit handling to avoid dividing by a peak of zero.

I learned that successful processing does not always require modifying the input data.

### Normalization Versus Clipping

I learned that normalization scales a complete waveform proportionally, while hard clipping limits individual samples and changes their relative amplitudes.

### Amplification and Attenuation

The normalization gain may be greater than or less than one.

A gain above one amplifies a quiet signal, while a gain below one attenuates a loud signal.

### Input Validation

Validating every sample before applying gain ensures that tested invalid inputs do not cause partial modifications to the buffer.

### Floating-Point Limitations

Very small nonzero sample values can produce very large normalization gain factors.

A future improvement is to calculate gain using higher precision and validate the result before modifying the buffer.

This would improve robustness for extremely quiet signals.

## 10. Key Takeaways

Day 9 introduced automatic amplitude adjustment into my C++20 audio engine.

I learned how to measure peak absolute amplitude, calculate normalization gain, and apply that gain to an entire signal.

I also gained experience with floating-point validation, silent-buffer handling, in-place processing, and automated testing.

The most important accomplishment was extending the existing audio pipeline to support normalization before PCM16 conversion.

My audio engine can now generate signals, combine multiple inputs, apply amplitude envelopes, normalize their output levels, and export the result as playable WAV audio.

## 11. Next Steps

The next development milestone is **Day 10 — Audio Levels, RMS, and Decibels**.

Potential tasks include:

- Understanding linear amplitude versus logarithmic decibel scales.
- Measuring audio levels in dBFS.
- Converting linear gain values into decibels.
- Converting decibel values into linear gain multipliers.
- Comparing peak levels and RMS levels.
- Expanding automated audio analysis tests.
- Preparing the audio-processing pipeline for more advanced speech synthesis.

These capabilities will help establish reliable audio measurements and provide a foundation for future loudness control and speech-model integration.

---

**Day 9 Result:** Successfully implemented and tested peak normalization, generated playable normalized audio, and verified that the final floating-point waveform reached a peak amplitude of approximately 0.9.
