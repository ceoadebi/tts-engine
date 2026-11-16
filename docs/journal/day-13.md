# Day 13 — Matrix–Vector Multiplication

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 16, 2026  
**Environment:** Ubuntu on WSL2, Clang  
**Status:** Both verification cases passed

## 1. Objective

I implemented a standalone C++20 experiment to calculate `y = W * x`. Matrix–vector multiplication is the weighted-sum step in a dense neural network layer. The goal was to calculate two results by hand, verify them against the program, and then change the input without changing the matrix or the implementation.

## 2. Matrix Layout and Implementation

I used a matrix with two rows and three columns:

```text
W = [  1   2   3 ]
    [ -1   0   2 ]
```

I stored it in one flat array in **row-major order**, with the first row followed by the second:

```text
Index:  0   1   2   3   4   5
Value:  1   2   3  -1   0   2
```

To access a matrix element, I used `row * cols + col`. Multiplying the row index by `cols` skips all the elements in the preceding rows. Adding `col` moves to the desired position within the current row. For example, row `1`, column `2` is at flat index `1 * 3 + 2 = 5`.

The outer loop visits each row and starts its sum at zero. The inner loop multiplies each matrix entry in that row by `x[col]` and adds the product to the sum. Once the inner loop finishes, the result goes in `y[row]`. The input vector starts at `x[0]` for every row.

## 3. Verification

| Run | Input vector `x` | Expected `y` | Observed `y` |
|---|---|---|---|
| 1 | `[2, 1, -1]` | `[1, -4]` | `[1, -4]` |
| 2 | `[1, 0, 0]` | `[1, -1]` | `[1, -1]` |

For the first run, the row sums were `2 + 2 - 3 = 1` and `-2 + 0 - 2 = -4`.

For the second run, I changed only `x`. The input `[1, 0, 0]` selects the first column because the other two entries in each row are multiplied by zero. This checks that the second row starts at flat index `3`. If it started at index `0` by mistake, the program would return `[1, 1]` instead of `[1, -1]`.

## 4. Reproduction

From the repository root, set `x` in `experiments/day13_matvec.cpp` to `[2, 1, -1]`, then compile and run:

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic \
    experiments/day13_matvec.cpp -o day13-matvec
./day13-matvec
```

Confirm the output is `1` and `-4`. Change only `x` to `[1, 0, 0]`, then recompile and rerun the same commands. Confirm the output is `1` and `-1`. This experiment does not require changes to CMake or the audio library.

## 5. Reflection and Next Step

The key distinction was between `cols`, the fixed width of a row, and `col`, the position within that row. Using `col` to calculate the row offset would make the starting position change as the inner loop advances. The second test made the indexing easier to verify because the expected output was visible directly in the matrix’s first column.

The next step is to add one bias value per output, extending `y = W * x` to `y = W * x + b`.
