# Day 8 — Audio Mixing and Gain Control

**Project:** C++20 Text-to-Speech Engine  
**Development Environment:** Ubuntu (WSL2), Clang, CMake, Ninja, Neovim  
**Status:** Implementation and testing complete

## 1. Objectives

The primary objective of Day 8 was to implement audio mixing and independent gain control within my C++20 text-to-speech engine.

During previous development sessions, I implemented an audio buffer, sine-wave generator, PCM16 converter, WAV writer, and amplitude envelope. Day 8 focused on combining these components to support multiple audio signals simultaneously.

The main objectives were:

- Understand audio mixing and the principle of superposition.
- Learn how clipping occurs when combining audio signals.
- Understand gain control and audio headroom.
- Design a reusable C++20 audio mixer.
- Implement independent gain control for multiple inputs.
- Preserve the original input buffers during mixing.
- Validate input parameters and handle invalid conditions.
- Write automated tests for the mixer.
- Generate, mix, and play two sine waves at different frequencies.

## 2. Audio Mixing Fundamentals

Audio mixing combines multiple audio signals into a single output signal.

The process is based on the principle of superposition, where corresponding audio samples are added together.

For two signals without additional gain:

\[
y[n] = x_A[n] + x_B[n]
\]

Where:

- \(x_A[n]\) is the sample from the first audio signal.
- \(x_B[n]\) is the sample from the second audio signal.
- \(y[n]\) is the resulting mixed sample.

To provide independent volume control, gain multipliers are introduced:

\[
y[n] = g_A x_A[n] + g_B x_B[n]
\]

Where \(g_A\) and \(g_B\) represent the gain applied to each input.

For example:

- Input A: 0.8
- Input B: 0.4
- Gain A: 0.5
- Gain B: 0.25

The mixed result is:

\[
y[n] = (0.8 \times 0.5) + (0.4 \times 0.25) = 0.5
\]

This allows individual signals to be attenuated or amplified independently before being combined.

## 3. Clipping and Audio Headroom

One important consideration during audio mixing is clipping.

In the current engine, floating-point audio is intended to remain within the normalized range of `-1.0` to `1.0` before PCM16 conversion.

For example, mixing samples of `0.7` and `0.6` at unity gain produces:

\[
0.7 + 0.6 = 1.3
\]

The result exceeds the normalized range.

When the audio is converted to PCM16, the existing `floatToPCM16()` implementation clamps values outside the valid range.

This creates hard clipping, which changes the waveform and can introduce audible distortion.

### Headroom

Headroom refers to the available amplitude range before reaching the clipping threshold.

One way to create headroom is to reduce the gain of individual signals before mixing.

For example:

\[
(0.7 \times 0.5) + (0.6 \times 0.5) = 0.65
\]

The resulting sample remains within the normalized range.

I designed the mixer to preserve floating-point values even when they exceed `1.0`, rather than automatically clipping them.

This separates the responsibility of mixing from the responsibility of output-level management.

## 4. Audio Mixer Implementation

I created the following files:

- `include/tts/audio/mixer.hpp`
- `src/audio/mixer.cpp`
- `tests/test_mixer.cpp`

The mixer exposes the following interface:

```cpp
bool mixAudio(
    AudioBuffer& output,
    const AudioBuffer& inputA,
    const AudioBuffer& inputB,
    float gainA,
    float gainB
);
```

The function accepts two input buffers, an output buffer, and independent gain values.

### Input Validation

Before mixing, the implementation verifies that:

- The input and output buffers contain the same number of samples.
- Both gain values are finite.
- Invalid input parameters are rejected before modifying the output buffer.

The function returns `false` if validation fails and `true` when mixing completes successfully.

### Mixing Algorithm

The mixer iterates over each sample index and calculates the weighted sum of the two inputs.

```cpp
for (std::size_t i{0}; i < output.size(); ++i) {
    output.data()[i] =
        inputA.data()[i] * gainA +
        inputB.data()[i] * gainB;
}
```

This produces a new output signal while preserving the original input samples.

The implementation uses an existing `AudioBuffer` for output instead of allocating additional memory inside the mixer.

## 5. Const-Correctness and AudioBuffer Improvements

One important C++ design improvement involved adding const-correct access to `AudioBuffer`.

Previously, the buffer exposed:

```cpp
float* data();
```

I added a second overload:

```cpp
const float* data() const;
```

The corresponding implementation is:

```cpp
const float* AudioBuffer::data() const {
    return m_data.get();
}
```

This allows the mixer to accept input buffers as `const AudioBuffer&`.

The input references prevent modification through the mixer interface, while the output remains a non-const reference.

This reinforces the distinction between read-only input data and mutable output data.

## 6. Automated Testing

I created `tests/test_mixer.cpp` and integrated it with CMake and CTest.

The test suite followed the same custom testing framework used during previous development sessions.

### Tests Performed

The tests verified:

- Correct sample-by-sample mixing.
- Independent gain control.
- Muting either input.
- Muting both inputs.
- Preserving mixed values above the normalized range.
- Reducing gain to maintain headroom.
- Preserving the original input buffers.
- Rejecting mismatched input and output buffer sizes.
- Rejecting NaN and infinite gain values.
- Accepting empty buffers.
- Cancellation of opposite-polarity signals.

### Test Results

**35 tests passed, 0 failed.**

The successful tests verified that the mixer produces the expected numerical results and handles invalid inputs without modifying the output buffer.

### Phase Cancellation

I also tested two signals containing opposite sample values:

- Signal A: `0.5`
- Signal B: `-0.5`

At unity gain:

\[
0.5 + (-0.5) = 0
\]

The resulting output contained silence.

This demonstrated how signals with opposite polarity can cancel when mixed together.

## 7. Audio Mixing Experiment

I created:

`experiments/day08_mixing.cpp`

The experiment combined two sine waves with different frequencies.

### Signal Parameters

| Property | Signal A | Signal B |
|---|---|---|
| Frequency | 440 Hz | 660 Hz |
| Sample rate | 48,000 Hz | 48,000 Hz |
| Duration | 1 second | 1 second |
| Initial amplitude | 0.8 | 0.8 |
| Mixing gain | 0.5 | 0.5 |

The experiment allocated three audio buffers:

- `inputA` for the first sine wave.
- `inputB` for the second sine wave.
- `output` for the mixed result.

Both input buffers were populated using the existing `generateSineWave()` function.

The signals were combined using `mixAudio()`, with independent gains of `0.5`.

### Amplitude Envelope

After mixing, I applied an amplitude envelope to the output buffer.

The envelope used:

- Attack: 0.1 seconds
- Sustain: 0.8 seconds
- Release: 0.1 seconds

This produced a gradual fade-in and fade-out for the combined signal.

### Audio Processing Pipeline

The completed processing pipeline was:

**Sine-Wave Generators → Audio Buffers → Mixer → Amplitude Envelope → PCM16 Conversion → WAV Writer**

The resulting file was:

`mixed_440_660.wav`

### Output Properties

| Property | Value |
|---|---|
| Sample rate | 48,000 Hz |
| Duration | 1 second |
| Channels | 1 (mono) |
| Output format | PCM16 WAV |
| Audio frames | 48,000 |
| Audio data size | 96,000 bytes |
| WAV header | 44 bytes |
| Expected total file size | 96,044 bytes |

The audio file was successfully generated and played.

I could hear the combined 440 Hz and 660 Hz tones, confirming that the mixing operation produced audible output from both input signals.

## 8. C++20 Concepts Applied

### Const References

The mixer uses `const AudioBuffer&` for read-only inputs and `AudioBuffer&` for the output.

This allows efficient access to existing buffers without copying or transferring ownership.

### Function Overloading

I added a const-qualified overload of `AudioBuffer::data()`, allowing the same member function name to support both mutable and immutable access.

### `std::size_t`

Sample indices use `std::size_t`, matching the type returned by `AudioBuffer::size()`.

### Floating-Point Validation

I used `std::isfinite()` from `<cmath>` to reject invalid gain values.

### Boolean Return Values

The mixer returns `bool` to indicate success or failure.

This allows calling code to detect errors before continuing with the audio-processing pipeline.

### Component Reuse

Rather than writing new code for signal generation, envelopes, or WAV export, I reused existing engine components.

This demonstrated the benefits of designing small, reusable modules with well-defined interfaces.

## 9. Challenges and Lessons Learned

### Understanding Gain

I reinforced the distinction between sample amplitude and gain.

Gain is a multiplier applied to an existing sample, rather than a separate audio value.

### Clipping Versus Mixing

I learned why mixing and clipping management should be treated as separate operations.

The mixer combines signals, while gain staging or a later normalization stage manages their final amplitude.

### Const-Correctness

Adding a const overload to `AudioBuffer::data()` improved the design of the engine and made it possible to read from const input buffers.

### Preserving Input Data

I learned to structure the mixer so that input signals remain unchanged while the output buffer receives the combined result.

### End-to-End Integration

The experiment demonstrated how several independent audio components can work together to create a more complex signal.

Instead of testing isolated functions only, I successfully generated, processed, exported, and listened to the resulting audio.

## 10. Key Takeaways

Day 8 expanded the engine from single-signal processing to multiple-signal audio generation.

I implemented a reusable mixer with independent gain controls, improved the const-correctness of the audio buffer, and verified the mixer using 35 automated checks.

I also gained a stronger understanding of signal superposition, headroom, phase cancellation, and how clipping can affect audio quality.

The most important accomplishment was integrating the sine-wave generators, mixer, amplitude envelope, PCM16 converter, and WAV writer into one functional pipeline.

My C++20 audio engine can now combine multiple generated signals and export the result as playable audio.

## 11. Next Steps

The next development milestone is **Day 9 — Audio Normalization and Clipping Management**.

Potential tasks include:

- Measuring peak amplitude across an audio buffer.
- Implementing peak normalization.
- Understanding normalization versus compression and limiting.
- Preventing unwanted PCM16 clipping.
- Comparing waveform amplitudes before and after normalization.
- Creating automated tests for gain processing.
- Integrating output-level management into the existing audio pipeline.

These capabilities will improve the reliability of the audio engine and prepare it for future speech synthesis and speech-model inference.

---

**Day 8 Result:** Successfully implemented and tested audio mixing with independent gain control, passed all 35 automated checks, and generated a playable WAV file combining 440 Hz and 660 Hz sine waves.
