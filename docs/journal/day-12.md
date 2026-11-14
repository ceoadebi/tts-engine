# Day 12 — Sanitizer Verification of the Audio Engine

**Project:** C++20 Text-to-Speech Engine  
**Environment:** Ubuntu on WSL2, Clang, CMake, Ninja, CTest  
**Status:** 10 of 10 CTest suites passed. No sanitizer diagnostics were found in the test log.

## 1. Objective

Before starting neural inference, I wanted a reproducible verification baseline for the audio engine. On Day 4, I used sanitizers while developing `AudioBuffer`. On Day 12, I ran the full audio test suite with both AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) enabled.

## 2. Test Baseline

I used `ctest --test-dir build -N` to list the tests registered in my regular build. CTest listed **10 suites**:

| # | CTest suite |
|---:|---|
| 1 | `AudioAnalyzerTests` |
| 2 | `AudioBufferTests` |
| 3 | `PCM16Tests` |
| 4 | `WavWriterTests` |
| 5 | `SineGeneratorTests` |
| 6 | `EnvelopeTests` |
| 7 | `MixerTests` |
| 8 | `NormalizerTests` |
| 9 | `DecibelsTests` |
| 10 | `GainTests` |

The `-N` flag only lists registered tests. It does not run them. Each registered suite can contain several individual checks.

## 3. Reproduction

I configured a separate Debug build in `build/day12-sanitize` so the verification did not change my regular build configuration. From the repository root:

```bash
cmake -S . -B build/day12-sanitize -G Ninja \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

cmake --build build/day12-sanitize

UBSAN_OPTIONS=halt_on_error=1 \
    ctest --test-dir build/day12-sanitize --output-on-failure
```

The compiler flags instrument the C++ code, including the object files in `tts_audio`. The linker flags ensure the test executables link against the sanitizer runtimes. Keeping frame pointers helps produce more useful stack traces.

## 4. Results

| Verification | Result |
|---|---|
| Registered CTest suites | 10 |
| Passed | 10 |
| Failed | 0 |
| Sanitizer diagnostics found in the test log | None |

I checked the full CTest log because `--output-on-failure` does not print the output of passing tests.

```bash
rg -n 'AddressSanitizer|LeakSanitizer|UndefinedBehaviorSanitizer|runtime error:' \
    build/day12-sanitize/Testing/Temporary/LastTest.log
```

The search returned no matching lines.

## 5. Troubleshooting

I initially got a `No such file or directory` error while setting up the separate build. Running the CMake configure step from the repository root before building resolved it. The `-B` option selects the directory where CMake writes the build files and cache. The later `cmake --build` command expects that directory to already be configured.

## 6. Reflection

A passing test suite and a clean sanitizer log answer different questions. The tests check whether the audio components behave as expected for specific inputs. The sanitizers check for certain memory errors and forms of undefined behavior while those tests run. A clean run gives me confidence in the paths I exercised, but it does not prove that untested inputs or code paths are safe.

## 7. Next Step

Day 13 moves into neural inference fundamentals with a small matrix–vector multiplication. I can rerun the Day 12 verification build as I add new components and use it to catch regressions in the audio code.
