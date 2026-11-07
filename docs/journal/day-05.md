# Day 5 — PCM16 Encoding and WAV File Generation

**Project:** C++20 Text-to-Speech Engine  
**Development Environment:** Ubuntu (WSL2), Clang, CMake, Ninja, Neovim  
**Status:** Implementation and testing complete

## 1. Objectives

The primary objective of Day 5 was to understand how digital audio samples are represented and stored in a standard WAV file.

Specific objectives included:

- Understand sample rates, audio frames, channels, and bit depth.
- Convert normalized floating-point audio samples into signed 16-bit PCM.
- Understand little-endian byte ordering.
- Implement a binary WAV file writer.
- Generate valid mono and stereo PCM16 WAV files.
- Verify audio output using automated tests and command-line tools.

## 2. Audio Fundamentals

Digital audio consists of discrete samples representing the amplitude of a signal at specific points in time.

The sample rate determines how many audio frames are processed per second. For example, a sample rate of 48,000 Hz means that 48,000 frames are processed every second.

For mono audio, each frame contains one sample. For stereo audio, each frame contains two samples.

The size of uncompressed audio can be calculated as:

**Audio Size = Sample Rate × Duration × Channels × Bytes per Sample**

For one second of mono PCM16 audio at 48 kHz:

48,000 × 1 × 1 × 2 = **96,000 bytes**

For stereo PCM16 audio at the same sample rate, the size doubles to 192,000 bytes.

## 3. PCM16 Conversion

I implemented `floatToPCM16()` in `src/audio/pcm.cpp`.

The function converts normalized floating-point values into signed 16-bit integers.

The valid PCM16 range is:

- Minimum: -32,768
- Maximum: 32,767

The conversion uses different scaling factors for negative and nonnegative values, allowing both endpoints of the PCM16 range to be represented.

The function also clamps samples outside the normalized range and handles non-finite inputs such as NaN and infinity.

Example conversions:

| Floating-Point Sample | PCM16 Value |
|---|---:|
| -1.0 | -32768 |
| -0.5 | -16384 |
| 0.0 | 0 |
| 0.5 | 16384 |
| 1.0 | 32767 |

I verified the converter using automated tests.

## 4. Little-Endian Binary Serialization

I implemented two helper functions in `src/audio/wav_writer.cpp`:

- `writeUint16LE()` — writes a 16-bit unsigned integer.
- `writeUint32LE()` — writes a 32-bit unsigned integer.

Both functions extract individual bytes using bit shifting and masking.

For example, the decimal value 48,000 is represented as:

`0x0000BB80`

In little-endian format, the bytes are written as:

`80 BB 00 00`

I created a binary-writing experiment and used `xxd` to inspect the resulting file, confirming that the bytes were written in the expected order.

These helpers are placed inside an anonymous namespace because they are implementation details that do not need to be exposed through the public header.

## 5. WAV File Writer Implementation

I implemented `writeWavPCM16()` using C++20.

The function accepts a file path, a span of floating-point audio samples, a sample rate, and a channel count.

The implementation performs the following operations:

1. Validates the sample rate and channel count.
2. Ensures the sample count contains complete audio frames.
3. Calculates the PCM16 data size.
4. Validates integer limits to prevent overflow.
5. Calculates block alignment and byte rate.
6. Opens an output file in binary mode.
7. Writes the 44-byte PCM WAV header.
8. Converts and writes each audio sample.
9. Closes the file and checks whether writing succeeded.

### WAV Header Structure

| Field | Size |
|---|---:|
| RIFF identifier | 4 bytes |
| RIFF chunk size | 4 bytes |
| WAVE identifier | 4 bytes |
| Format chunk | 24 bytes |
| Data chunk identifier | 4 bytes |
| Data size | 4 bytes |
| **Total header** | **44 bytes** |

The writer supports mono and interleaved stereo PCM16 audio.

## 6. C++20 Concepts Applied

**`std::span<const float>`**

Provides a non-owning view of audio samples without requiring a separate pointer and sample-count parameter.

**`std::filesystem::path`**

Represents output file paths using the standard C++ filesystem library.

**Fixed-width integer types**

Types such as `std::uint16_t`, `std::uint32_t`, and `std::int16_t` ensure that binary fields have the required sizes.

**`std::numeric_limits`**

Allows the writer to validate whether calculated WAV header values fit within the available integer ranges.

**`static_cast` and `reinterpret_cast`**

I used explicit casts to convert numeric values safely and provide byte addresses to the binary output stream.

## 7. Testing and Verification

I created `tests/test_wav_writer.cpp` and integrated it into CMake and CTest.

The tests covered:

- Writing a valid mono PCM16 WAV file.
- Writing a valid stereo PCM16 WAV file.
- Verifying RIFF and WAVE identifiers.
- Verifying format and data chunk fields.
- Checking sample rate, byte rate, and block alignment.
- Checking the exact bytes of encoded audio samples.
- Rejecting invalid sample rates and channel counts.
- Rejecting incomplete stereo frames.
- Handling invalid output paths.

I also inspected the generated WAV file using:

`xxd -g 1`

and:

`file`

The `file` utility recognized the output as a 16-bit mono PCM WAV file at 48 kHz.

My five-sample mono WAV test produced the expected file size of **54 bytes**, consisting of 44 header bytes and 10 audio data bytes.

The automated WAV writer test suite passed.

## 8. Challenges and Debugging

One challenge was understanding the difference between bits and bytes when calculating byte rate and block alignment.

Another was distinguishing 16-bit and 32-bit fields in the WAV header.

I also encountered a compiler ambiguity involving the `writeUint32LE()` helper. I resolved the build problem and learned why private helper functions should be organized consistently within an anonymous namespace.

Testing the output at the byte level helped me verify that the WAV structure was correct rather than relying only on successful compilation.

## 9. Key Takeaways

Day 5 connected several important systems programming concepts to a practical audio application.

I learned how floating-point audio samples are converted into a binary representation, how integers are serialized in little-endian order, and how a file format defines the structure of binary data.

I also gained more experience with C++20 interfaces, input validation, binary file I/O, automated testing, and debugging compiler errors.

Most importantly, the TTS engine can now write valid PCM16 WAV files without relying on an external audio-file writing library.

## 10. Next Steps

The next development milestone is to generate meaningful audio signals rather than using only fixed test samples.

Potential tasks include:

- Generate sine-wave audio signals.
- Investigate waveform frequency, amplitude, and duration.
- Produce playable WAV files.
- Explore audio playback and buffering.
- Prepare the audio pipeline for future speech-model integration.

**Day 5 Result:** Successfully implemented and tested PCM16 conversion and WAV file generation.
