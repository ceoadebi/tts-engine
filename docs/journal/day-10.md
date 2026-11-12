# Day 10 — Audio Levels, RMS, and Decibels (dBFS)

**Project:** C++20 Text-to-Speech Engine  
**Development Environment:** Ubuntu 24.04 (WSL2), Clang, CMake, Ninja, Neovim  
**Date:** November 12, 2026  
**Status:** Technical implementation and testing complete

---

## 1. Objectives

The objective of Day 10 was to implement decibel-based audio measurements and integrate them into my existing C++20 audio-processing engine.

During Day 9, I implemented peak normalization, allowing audio signals to be scaled to a target amplitude before PCM16 conversion.

Day 10 expanded this functionality by introducing decibels relative to full scale (dBFS), a logarithmic representation of digital audio levels.

The main objectives were:

- Understand the relationship between linear amplitude and decibels.
- Implement conversions between linear amplitude and dBFS.
- Handle digital silence, infinity, NaN, and numerical overflow.
- Develop automated tests for both conversion functions.
- Measure peak and RMS levels from an exported PCM16 WAV file.
- Integrate decibel measurements with the existing audio analyzer.
- Verify floating-point measurements against PCM16 measurements.
- Export a WAV file from the integrated audio-processing pipeline.

## 2. Understanding Decibels and dBFS

Decibels express ratios on a logarithmic scale.

In digital audio, dBFS stands for **decibels relative to full scale**.

For the floating-point representation used in this project, an amplitude magnitude of `1.0` serves as the full-scale reference.

The conversion from linear amplitude to dBFS is:

\[
L_{\mathrm{dBFS}} = 20\log_{10}(A)
\]

Where:

- \(L_{\mathrm{dBFS}}\) is the signal level in decibels.
- \(A\) is the nonnegative linear amplitude magnitude.

### Example Values

| Linear amplitude | Level (dBFS) |
|---|---|
| 2.0 | +6.0206 |
| 1.0 | 0.0000 |
| 0.5 | −6.0206 |
| 0.1 | −20.0000 |
| 0.01 | −40.0000 |
| 0.001 | −60.0000 |

An amplitude magnitude of `1.0` corresponds to `0 dBFS`.

Magnitudes below `1.0` produce negative dBFS values, while magnitudes above `1.0` produce positive dBFS values.

Floating-point audio can temporarily exceed full scale during intermediate processing. However, signals must be brought into the supported range before conversion to PCM16 to avoid hard clipping.

### Digital Silence

Digital silence has an amplitude of zero.

Because the logarithm of zero is undefined, digital silence is represented as negative infinity dBFS:

\[
0 \longrightarrow -\infty\ \mathrm{dBFS}
\]

This required special handling in the conversion function.

## 3. Converting dBFS to Linear Amplitude

I derived the inverse conversion equation from the original logarithmic relationship.

Starting with:

\[
dB = 20\log_{10}(A)
\]

Dividing by 20:

\[
\frac{dB}{20} = \log_{10}(A)
\]

Applying the inverse logarithm:

\[
A = 10^{dB/20}
\]

This equation allows an audio engine to convert a decibel level into a linear amplitude multiplier.

For example:

\[
A = 10^{-20/20} = 0.1
\]

Therefore, `−20 dBFS` corresponds to a linear amplitude of `0.1`.

These conversions will eventually support gain controls, level meters, and amplitude management in the TTS engine.

## 4. Decibel Conversion Module

I created three new files:

- `include/tts/audio/decibels.hpp`
- `src/audio/decibels.cpp`
- `tests/test_decibels.cpp`

The public interface contains two functions:

```cpp
double amplitudeToDbFS(double amplitude);
double dbFSToAmplitude(double db);
```

### 4.1 Amplitude to dBFS

The first function converts a nonnegative linear amplitude magnitude into decibels.

The implementation uses `std::log10()` from `<cmath>`.

Before performing the calculation, the function validates that the input is finite and nonnegative.

Invalid values return a quiet NaN using `std::numeric_limits<double>::quiet_NaN()`.

Zero amplitude is handled separately and returns negative infinity.

The core calculation is:

```cpp
double result = 20.0 * std::log10(amplitude);
return result;
```

This allows the engine to represent amplitudes on a logarithmic scale.

### 4.2 dBFS to Amplitude

The second function converts decibels back into linear amplitude.

The implementation uses:

```cpp
double amplitude = std::pow(10.0, db / 20.0);
```

The function handles several special cases:

- Negative infinity returns `0.0`, representing silence.
- NaN is rejected.
- Positive infinity is rejected.
- Finite inputs producing an infinite result are rejected.

This last validation prevents the function from returning an unusable infinite amplitude when the exponent exceeds the representable range of `double`.

### 4.3 C++ Concepts Applied

This module reinforced several C++ concepts:

**Floating-point classification:** I used `std::isfinite()`, `std::isinf()`, and `std::isnan()` to distinguish valid values from special floating-point representations.

**Mathematical operations:** I used `std::log10()` and `std::pow()` for logarithmic and exponential calculations.

**Numerical limits:** I used `std::numeric_limits<double>` to represent NaN and infinity.

**Double precision:** I used `double` for decibel calculations to provide more precision than the underlying `float` audio samples.

**Input validation:** I validated values before using them in calculations that could produce invalid results.

## 5. Automated Testing

I implemented a dedicated test file:

`tests/test_decibels.cpp`

The test suite verifies:

- Known amplitude-to-dBFS conversions.
- Known dBFS-to-amplitude conversions.
- Conversion of silence to negative infinity.
- Conversion of negative infinity back to silence.
- Negative amplitude rejection.
- NaN input rejection.
- Positive and negative infinity handling.
- Overflow handling.
- Round-trip conversion accuracy.

### Round-Trip Testing

Round-trip testing verifies that converting a value to another representation and back recovers approximately the original value.

For amplitude:

\[
A \rightarrow dBFS \rightarrow A
\]

For decibels:

\[
dBFS \rightarrow A \rightarrow dBFS
\]

Because floating-point calculations introduce small rounding differences, the tests use approximate comparisons instead of requiring exact equality.

### Test Results

| Measurement | Result |
|---|---|
| Total assertions | 38 |
| Passed | 38 |
| Failed | 0 |

All automated decibel conversion tests passed.

### Debugging and Improvements

During code review, I identified that a finite decibel input could still produce infinity through `std::pow()`.

I addressed this by validating the computed result before returning it.

I also encountered a compiler error later in the analyzer integration experiment because the example code treated `generateSineWave()` as a function returning `bool`.

The actual interface returns `void`.

I corrected the experiment to call `generateSineWave()` directly without testing its nonexistent return value.

This reinforced the importance of verifying existing function interfaces before integrating components.

## 6. PCM16 WAV Audio Level Analysis

After implementing the decibel conversion module, I created:

`experiments/day10_audio_levels.cpp`

This experiment analyzed an existing WAV file generated during Day 9:

`mixed_440_660_normalized.wav`

The program opened the file in binary mode, inspected its WAV header, decoded the PCM16 samples, and calculated peak and RMS amplitudes.

It then converted both measurements into dBFS.

### Peak Amplitude

Peak amplitude represents the largest absolute sample magnitude:

\[
P = \max_n |x[n]|
\]

It indicates how close the signal comes to the full-scale amplitude boundary.

### RMS Amplitude

Root mean square (RMS) measures the square root of the average squared sample magnitude:

\[
RMS = \sqrt{\frac{1}{N}\sum_{n=0}^{N-1}x[n]^2}
\]

RMS is useful for describing a signal's overall level over a measurement interval.

Although RMS can be related to perceived loudness, it is not itself a perceptual loudness measurement such as LUFS.

### PCM16 Measurement Results

| Measurement | Result |
|---|---:|
| Sample rate | 48,000 Hz |
| Samples | 48,000 |
| Duration | 1.000000 seconds |
| Peak amplitude | 0.899994 |
| Peak level | −0.915209 dBFS |
| RMS amplitude | 0.439599 |
| RMS level | −7.138866 dBFS |

The experiment completed successfully.

The measured peak was very close to the Day 9 target amplitude of `0.9`.

The small difference resulted from converting floating-point audio into discrete PCM16 sample values.

## 7. Integration with the Existing Audio Analyzer

Next, I integrated the new decibel conversion module with my existing audio analyzer.

I created:

`experiments/day10_analyzer_integration.cpp`

The existing analyzer already supported:

```cpp
bool findMaxSample(
    const float* samples,
    int count,
    float* result
);

bool findMinSample(
    const float* samples,
    int count,
    float* result
);

bool calculateRMS(
    const float* samples,
    int count,
    float* result
);
```

Instead of duplicating these calculations, I reused the existing functions to analyze the audio directly in memory.

### Peak Calculation

One important consideration was that the largest signed sample is not necessarily the largest absolute sample.

For example, if the minimum sample is `−0.95` and the maximum sample is `0.70`, the peak amplitude is `0.95`.

I calculated the absolute peak from both extrema:

```cpp
const float peak = std::max(
    std::abs(minSample),
    std::abs(maxSample)
);
```

The peak and RMS amplitudes were then passed to `amplitudeToDbFS()`.

### Integrated Processing Pipeline

The complete pipeline was:

1. Generate 440 Hz and 660 Hz sine waves.
2. Store the waveforms in `AudioBuffer` instances.
3. Mix the two signals.
4. Apply the attack and release envelope.
5. Normalize the mixed output to a peak amplitude of `0.9`.
6. Calculate the minimum and maximum samples.
7. Calculate RMS amplitude.
8. Convert peak and RMS amplitudes to dBFS.
9. Export the processed audio as a PCM16 WAV file.

This architecture allows the TTS engine to measure signal levels without reopening a previously exported file.

## 8. Analyzer Integration Results

The integrated experiment produced the following results:

| Measurement | Result |
|---|---:|
| Minimum sample | −0.9 |
| Maximum sample | 0.9 |
| Peak amplitude | 0.9 |
| RMS amplitude | 0.439604 |
| Peak level | −0.915149 dBFS |
| RMS level | −7.13877 dBFS |
| WAV export | Successful |

The experiment successfully exported:

`day10_analyzed_audio.wav`

### Floating-Point Versus PCM16 Measurements

The analyzer integration measured the floating-point signal before PCM16 conversion.

The standalone WAV experiment measured samples after PCM16 conversion.

These measurements were closely aligned.

The peak amplitude differed slightly because PCM16 conversion maps floating-point samples to discrete integer values.

This demonstrated that the original audio-processing pipeline maintained consistent levels through conversion and export.

## 9. Peak-to-RMS Difference

The difference between peak and RMS levels describes the separation between the highest sample magnitude and the signal's RMS level.

For the integrated experiment:

\[
-0.915149 - (-7.13877) \approx 6.224\ \mathrm{dB}
\]

The peak-to-RMS difference was approximately **6.22 dB**.

This measurement describes the signal's crest factor in decibels over the analyzed interval.

Crest factor can help characterize differences between transient peaks and the overall signal level.

It is not a direct measurement of perceived loudness.

## 10. Challenges and Lessons Learned

### Logarithmic Versus Linear Representation

I learned that decibels are logarithmic rather than linear.

A tenfold change in amplitude corresponds to a 20 dB change.

### Positive dBFS Values

I initially expected an amplitude above `1.0` to produce a negative dBFS value.

Working through the logarithmic relationship clarified that amplitudes greater than the reference produce positive dBFS values.

This is useful for detecting signals that could clip during PCM16 conversion.

### Numerical Edge Cases

The conversion functions required explicit handling of silence, NaN, infinity, and overflow.

I learned that even a finite input can produce a nonfinite result after mathematical processing.

### Absolute Peak Versus Signed Maximum

I reinforced the distinction between the largest signed sample and the largest absolute magnitude.

Accurate peak measurement must consider both positive and negative waveform excursions.

### Reusing Existing Components

Rather than implementing another complete RMS analyzer, I reused the existing `calculateRMS()` function.

This reduced code duplication and improved modularity.

### API Compatibility

The integration build exposed a mismatch between an example's assumptions and the actual return type of `generateSineWave()`.

Correcting the function call reinforced the importance of respecting existing interfaces.

## 11. Final Technical Accomplishments

By the end of Day 10, the audio engine supported:

- Floating-point audio generation.
- Audio mixing with independent gain controls.
- Attack and release envelope processing.
- Peak normalization.
- Peak-amplitude measurement.
- RMS-amplitude measurement.
- Conversion between amplitude and dBFS.
- Handling of invalid numerical inputs.
- PCM16 conversion.
- WAV serialization.
- Direct in-memory audio analysis.
- Automated testing of decibel calculations.

The most significant improvement was the ability to express and inspect audio levels using the logarithmic units commonly used in audio engineering.

## 12. Future Improvements

Potential improvements include:

- Creating a reusable audio-level measurement interface that combines peak and RMS calculations.
- Reducing code duplication between WAV analysis and in-memory analysis.
- Adding decibel-based gain controls.
- Supporting block-by-block audio-level measurement.
- Investigating numerical underflow for extremely negative decibel values.
- Learning about true-peak measurements and inter-sample peaks.
- Eventually introducing LUFS-based loudness measurement.

These features will become increasingly important when integrating machine-learning speech models and preparing generated speech for playback.

## 13. Conclusion

Day 10 successfully introduced decibel-based audio measurements into my C++20 text-to-speech engine.

I implemented mathematical conversions between linear amplitude and dBFS, developed a test suite with 38 passing assertions, and verified the accuracy of the conversions through known values and round-trip tests.

I also analyzed PCM16 WAV samples and integrated the new conversions with my existing audio analyzer.

The final integrated experiment measured a peak of approximately `−0.915 dBFS`, an RMS level of approximately `−7.139 dBFS`, and successfully exported a playable WAV file.

**Day 10 Result:** Decibel conversion, automated testing, PCM16 level analysis, and direct analyzer integration completed successfully.

---

**Next Milestone:** Day 11 — Audio Gain Control and Signal-Level Management.
