# Day 14 — Adding Bias to a Dense Layer

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 17, 2026  
**Environment:** Ubuntu on WSL2, Clang  
**Status:** Both verification cases passed

## 1. Objective

On Day 13, I implemented matrix–vector multiplication as `y = W * x`. On Day 14, I extended that experiment to compute `y = W * x + b`. The goal was to add one bias per output and verify that each bias was applied exactly once.

## 2. Implementation

I kept the Day 13 experiment separate and used `experiments/day14_dense.cpp` for the new version. The matrix stayed the same, and I added a two-element bias vector:

```text
W = [  1   2   3 ]    b = [ 0.5 ]
    [ -1   0   2 ]        [ 1.5 ]
```

The matrix still uses row-major storage. The outer loop visits one row at a time, and the inner loop accumulates the weighted products for that row. After the inner loop finishes, I add `b[row]` to the completed sum and store the result in `y[row]`.

The bias vector has two elements because the matrix has two rows and produces two outputs. Its length is determined by the number of outputs, not the number of input elements.

## 3. Verification

I kept the weights, biases, and loops unchanged between runs and changed only the input vector.

| Run | Input `x` | Dot products `W * x` | Expected `y` | Observed `y` |
|---|---|---|---|---|
| 1 | `[2, 1, -1]` | `[1, -4]` | `[1.5, -2.5]` | `[1.5, -2.5]` |
| 2 | `[0, 0, 0]` | `[0, 0]` | `[0.5, 1.5]` | `[0.5, 1.5]` |

The first run checked that the bias was added to the dot products I had already verified on Day 13. The second run isolated the bias. With a zero input, every weighted product is zero, so the output should equal `b`.

The terminal output still used the Day 13 label `y = Wx`. The numbers matched `y = Wx + b`. The label should be updated to reflect the new calculation.

## 4. Reproduction

From the repository root, set `x` in `experiments/day14_dense.cpp` to `[2, 1, -1]`, then compile and run:

```bash
clang++ -std=c++20 -Wall -Wextra -Wpedantic \
    experiments/day14_dense.cpp -o day14-dense
./day14-dense
```

Confirm that the program prints `1.5` and `-2.5`. Change only `x` to `[0, 0, 0]`, then recompile and rerun. Confirm that it prints `0.5` and `1.5`.

This is a standalone experiment. It does not require CMake changes, ONNX Runtime, or changes to the audio library.

## 5. Reflection

The inner loop runs once per input element, but the bias belongs to the output. If I added it inside the inner loop, it would be added three times per row. The zero-input test would then produce `[1.5, 4.5]` instead of `[0.5, 1.5]`. Placing the bias addition after the inner loop keeps it tied to the completed dot product.

This experiment implements the weighted sum and bias step of a dense layer. An activation function, if needed, would apply to each result after that step.
