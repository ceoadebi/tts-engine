# Day 11 — Applying Gain in Decibels

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 13, 2026  
**Environment:** Ubuntu on WSL2, Clang, CMake, Ninja  
**Status:** Implementation, automated tests, and integration benchmark passed

## 1. Objective

On Day 10, I used decibels to measure peak and RMS levels. On Day 11, I used decibels to change those levels. The goal was to apply gain to an existing `AudioBuffer`, verify the change before PCM16 conversion, and export the attenuated signal as a WAV file.

## 2. Gain versus normalization

A gain of −6.0206 dB is a relative adjustment. A reading in dBFS describes a signal level relative to full scale. I reused the Day 10 conversion to turn the requested gain into a linear multiplier:

```text
multiplier = 10^(gainDb / 20)
```

A gain of −6.0206 dB gives a multiplier of approximately `0.5`, so it halves both peak and RMS amplitude. Peak normalization works differently. It chooses a multiplier based on the signal’s measured peak and the requested target.

Processing order matters. Normalizing to `0.9` and then halving the signal gives a peak of `0.45`. If I halve the signal first and normalize afterward, the normalizer brings the non-silent signal’s peak back to `0.9`.

## 3. Implementation

| File | Purpose |
|---|---|
| `include/tts/audio/gain.hpp` | Declares `applyGainDb()` |
| `src/audio/gain.cpp` | Validates and applies gain in place |
| `tests/test_gain.cpp` | Verifies successful gain and failure behavior |
| `experiments/day11_gain.cpp` | Measures levels before and after gain, then exports WAV |
| `CMakeLists.txt` | Builds the gain component, test, and experiment |

`applyGainDb()` uses `dbFSToAmplitude()` to calculate a `double` multiplier. Invalid conversions return `false` without changing the buffer. Under the current policy, negative infinity dB maps to a multiplier of zero and mutes finite samples.

The function uses two passes:

1. **Validate.** Compute each scaled sample as a `double` without writing it back. Reject nonfinite results and magnitudes greater than `std::numeric_limits<float>::max()`.
2. **Apply.** Only after every sample passes, recompute each scaled sample, cast it to `float`, and store it in the buffer.

This avoids allocating a second buffer and prevents a failure on the last sample from leaving earlier samples modified.

The gain stage does not clamp samples at `±1.0`. A boost can take a sample from `0.6` to `1.2`, and a later attenuation can bring it back to approximately `0.6`. If the gain stage clipped `1.2` to `1.0` first, halving it would give `0.5` instead. The PCM16 converter still clamps samples that exceed full scale at export.

## 4. Automated testing

**Result: 9 checks passed, 0 failed.**

The tests covered unity gain, attenuation, boosting above full scale without clipping, muting, invalid gain inputs, and positive and negative sample overflow.

For the late-overflow tests, I used `0.5f` as the first sample and the largest finite `float` or its negative as the second. Applying approximately double gain leaves the first result representable but pushes the second outside the `float` range. The tests check that the function returns `false` and that **both original samples remain unchanged**. Checking the first sample catches an implementation that writes as it validates.

## 5. Integration benchmark

I generated one second of 440 Hz and 660 Hz sine waves at 48 kHz, each with an amplitude of `0.8`. I mixed the signals with gains of `0.5` each, applied a `0.1`-second attack and release, and normalized the mix to a peak of `0.9`. I measured the levels, applied `−6.0206 dB`, and measured them again before WAV export.

| Measurement | Before gain | After gain | Change |
|---|---:|---:|---:|
| Peak amplitude | 0.900000 | 0.450000 | −6.020600 dB |
| RMS amplitude | 0.439604 | 0.219802 | −6.020600 dB |
| Sample count | 48,000 | 48,000 | Unchanged |

Both level changes matched the requested gain within the experiment’s **0.001 dB tolerance**. The experiment successfully wrote `day11_attenuated.wav`. Measuring before export kept PCM16 quantization out of the comparison.

The changes are reported in **dB**, not dBFS, because they are differences between readings taken against the same full-scale reference.

## 6. Reproduction

Run from the repository root using the toolchain in [Development Setup](../setup.md):

```bash
cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=clang++
cmake --build build
./build/test-gain
ctest --test-dir build -R '^GainTests$' --output-on-failure
./build/day11-gain
```

`test-gain` reports nine individual checks. CTest runs the executable as one registered test.

To play the exported file with FFmpeg installed:

```bash
ffplay -nodisp -autoexit day11_attenuated.wav
```

## 7. Reflection

The main lesson was that validating and writing in the same pass cannot guarantee that the buffer stays unchanged on failure. Separating those steps makes the guarantee explicit.

I also learned to distinguish the numeric range of a `float` from the `±1.0` full-scale boundary at PCM16 export. Keeping those limits separate lets the engine preserve peaks that a later processing stage can still bring back into range.
