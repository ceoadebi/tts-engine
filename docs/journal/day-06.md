# Day 6 — Audio Signal Generation and Frequency Verification

**Project:** C++20 Text-to-Speech Engine  
**Development Environment:** Ubuntu (WSL2), Clang, CMake, Ninja, Neovim  
**Status:** Implementation and testing complete

## 1. Objectives

The primary objective of Day 6 was to generate an actual audio signal using the components developed during previous sessions.

The main objectives were:

- Understand the mathematical representation of a sine wave.
- Convert continuous time into discrete sample indices.
- Implement a reusable sine-wave generator in C++20.
- Integrate the generator with the existing `AudioBuffer`.
- Generate a valid, playable PCM16 WAV file.
- Verify waveform amplitude, RMS, and frequency.
- Implement automated tests using CTest.

## 2. Sine-Wave Fundamentals

A sine wave is a periodic signal that oscillates between positive and negative amplitudes.

The continuous-time equation is:

**x(t) = A × sin(2πft + φ)**

Where:

- **A:** Amplitude
- **f:** Frequency in Hertz
- **t:** Time in seconds
- **φ:** Initial phase in radians

For this implementation, the initial phase was zero.

Because digital audio consists of discrete samples, continuous time must be represented using a sample index.

The relationship between the sample index and time is:

**t = n / fs**

Where `n` is the sample index and `fs` is the sample rate.

Substituting this relationship into the sine-wave equation produces:

**x[n] = A × sin(2πfn / fs)**

For the initial signal:

- Frequency: 440 Hz
- Amplitude: 0.8
- Sample rate: 48,000 Hz
- Duration: 1 second
- Channels: 1 (mono)

The total number of samples was:

**48,000 × 1 = 48,000 samples**

## 3. Sine-Wave Generator Implementation

I created two new files:

- `include/tts/audio/sine_generator.hpp`
- `src/audio/sine_generator.cpp`

The generator exposes the following function:

```cpp
void generateSineWave(
    AudioBuffer& buffer,
    double frequency,
    double amplitude,
    std::uint32_t sampleRate
);
```

The function accepts an existing `AudioBuffer` by reference and fills it with generated sine-wave samples.

Using a reference avoids copying the buffer and allows the function to modify the original audio data.

### Input Validation

The generator checks the following conditions:

- The sample rate must be greater than zero.
- Frequency and amplitude must be finite.
- Frequency must be positive and below the Nyquist frequency.
- Amplitude must be between 0.0 and 1.0.

Invalid inputs cause the function to return without modifying the buffer.

### Sample Generation

The implementation iterates over each sample in the buffer.

For every sample index, the generator:

1. Calculates the corresponding timestamp.
2. Calculates the sine-wave phase.
3. Evaluates the sine function.
4. Converts the result to `float`.
5. Stores the sample in the audio buffer.

The main generation loop is:

```cpp
for (std::size_t i{0}; i < buffer.size(); ++i) {
    const double t =
        static_cast<double>(i) / sampleRate;

    const double phase =
        2.0 * std::numbers::pi * frequency * t;

    const float sample = static_cast<float>(
        amplitude * std::sin(phase)
    );

    buffer.data()[i] = sample;
}
```

Using `double` for intermediate calculations provides additional precision, while storing the samples as `float` reduces audio-buffer memory usage.

## 4. Integration with the Audio Pipeline

I created an experiment in:

`experiments/day06_sine_wave.cpp`

The experiment connects the sine-wave generator to the audio components implemented previously.

The signal passes through the following pipeline:

**Sine Generator → AudioBuffer → PCM16 Converter → WAV Writer**

The generated samples are accessed using `buffer.data()` and `buffer.size()`.

A `std::span<const float>` provides a non-owning view of these samples, which is passed to `writeWavPCM16()`.

The experiment generated a file named:

`sine_440hz.wav`

### Output Properties

| Property | Value |
|---|---|
| Sample rate | 48,000 Hz |
| Frequency | 440 Hz |
| Amplitude | 0.8 |
| Channels | 1 |
| Bit depth | 16-bit PCM |
| Duration | 1 second |
| Audio samples | 48,000 |
| Audio data size | 96,000 bytes |
| WAV header | 44 bytes |
| Total WAV size | 96,044 bytes |

I confirmed that the file had the expected size and successfully played the audio.

The recording produced a steady, audible tone.

## 5. C++20 Concepts Applied

### `std::numbers::pi`

I used the C++20 `<numbers>` library to access a standard representation of π.

This eliminated the need to define a custom constant for mathematical calculations.

### References

The generator accepts `AudioBuffer&`, allowing it to modify an existing buffer without transferring ownership or copying audio samples.

### `std::span`

The WAV-writing experiment uses `std::span<const float>` to pass audio samples to the WAV writer.

A span provides access to contiguous memory while retaining information about the number of elements.

### Explicit Type Conversion

I used `static_cast<double>` when calculating timestamps to avoid integer division.

I also used `static_cast<float>` when storing calculated samples in the audio buffer.

### Standard Mathematical Functions

The generator uses functions from `<cmath>`, including:

- `std::sin()` for sine-wave generation.
- `std::isfinite()` for input validation.
- `std::abs()` and `std::sqrt()` in waveform tests.

## 6. Testing and Verification

I implemented automated tests in:

`tests/test_sine_generator.cpp`

The test executable was integrated with CMake and CTest.

### Automated Test Results

All 12 checks passed.

The tests verified:

- Correct sample count.
- Zero-valued initial sample.
- Individual samples matching the sine-wave equation.
- Samples remaining within amplitude limits.
- Expected RMS.
- Estimated signal frequency.
- Silence when amplitude equals zero.
- Buffer preservation when the sample rate is invalid.
- Rejection of a frequency at the Nyquist limit.

### RMS Verification

The theoretical RMS of a sine wave is:

**RMS = A / √2**

For an amplitude of 0.8:

**RMS ≈ 0.5657**

The generated waveform passed the RMS verification test.

### Frequency Verification

I implemented a basic frequency estimator using positive-going zero crossings.

A positive-going crossing occurs when:

```cpp
previous < 0.0f && current >= 0.0f
```

The algorithm counts these crossings and divides the result by the signal duration.

**Estimated Frequency = Positive-Going Crossings / Duration**

For the one-second, 440 Hz waveform, the estimated frequency was within the test's tolerance of ±1 Hz.

The discrete counting method is approximate because the signal begins at time zero and the recording contains a finite number of samples.

The frequency test passed.

## 7. Challenges and Debugging

### Integer Division

One important concept was recognizing that dividing two integer values produces an integer result in C++.

For example:

```cpp
double t = i / sampleRate;
```

This expression can discard the fractional portion of the timestamp.

The solution was to convert one operand to `double` before division.

### Float Versus Double

I learned why using `double` for intermediate waveform calculations can improve numerical precision without requiring the audio buffer itself to store 64-bit samples.

A one-second mono recording at 48 kHz requires approximately 192,000 bytes when stored as 32-bit floating-point samples, compared with 384,000 bytes when stored as 64-bit floating-point samples.

### References and Pointers

I reviewed the distinction between references and pointers in C++.

Using `AudioBuffer&` communicates that the generator requires an existing valid buffer and does not take ownership of it.

### Frequency Estimation

I learned that counting zero crossings provides a simple way to estimate the frequency of a clean periodic signal.

However, the precision depends on the observation duration and the treatment of waveform boundaries.

This method is sufficient for basic signal verification but is not a complete frequency-analysis solution.

## 8. Key Takeaways

Day 6 demonstrated how mathematical signal models can be translated into executable C++ code.

I learned how to use sample indices and sample rates to generate discrete-time audio, how to manage floating-point precision, and how to verify the resulting signal mathematically.

The most important accomplishment was integrating multiple previously developed components into one functional audio pipeline.

The engine can now generate and export audible audio signals using its own sine-wave generator, audio buffer, PCM16 conversion function, and WAV writer.

## 9. Next Steps

The next milestone is to expand the audio engine beyond a constant-amplitude sine wave.

Potential areas of development include:

- Generating additional waveform types.
- Implementing amplitude envelopes.
- Understanding signal discontinuities and audio clicks.
- Exploring audio mixing and gain control.
- Investigating real-time audio playback.
- Preparing the pipeline for speech-model inference.

These capabilities will form the foundation for more advanced audio synthesis and eventual text-to-speech output.

---

**Day 6 Result:** Successfully implemented, tested, exported, and played a 440 Hz sine wave using the C++20 audio engine.
