# Day 17 — Map the Two-Layer Model to an ONNX Graph

**Project:** C++20 Text-to-Speech Engine  
**Date:** November 19, 2026  
**Status:** Graph design and hand verification complete. Model export and runtime verification pending.

## 1. Objective

On Day 16, I implemented a two-layer forward pass in C++20. On Day 17, I mapped that same calculation to an ONNX graph. The goal was to define the input and output shapes and establish reference values before running the model in an inference runtime.

The weights and biases stay fixed. This exercise is about inference, not training.

## 2. Model Definition

```text
W1 = [  1   2   3 ]    b1 = [ 0.5 ]
     [ -1   0   2 ]         [ 1.5 ]

W2 = [ 2  -1 ]         b2 = [ 0.25 ]

h = ReLU(W1x + b1)
y = W2h + b2
```

The model uses `float32` tensors. I kept `x` as a column vector with shape `[3, 1]`, matching the notation from Day 16. The first layer produces two values with shape `[2, 1]`. The second layer produces one value with shape `[1, 1]`.

## 3. Graph and Reference Trace

The graph has five operations: `MatMul → Add → Relu → MatMul → Add`. Each bias has the same shape as the output it is added to, so the addition does not change the shape. ReLU is applied only between the two layers.

In the table, lists of values are shorthand for column tensors. The shape column gives the full tensor dimensions.

| Operation | Input shapes | Output shape | For `x = [2, 1, -1]` | For `x = [0, 0, 0]` |
|---|---|---|---|---|
| `MatMul(W1, x)` | `[2, 3] × [3, 1]` | `[2, 1]` | `[1, -4]` | `[0, 0]` |
| `Add` first bias `b1` | `[2, 1] + [2, 1]` | `[2, 1]` | `[1.5, -2.5]` | `[0.5, 1.5]` |
| `Relu` | `[2, 1]` | `[2, 1]` | `[1.5, 0]` | `[0.5, 1.5]` |
| `MatMul(W2, h)` | `[1, 2] × [2, 1]` | `[1, 1]` | `[3.0]` | `[-0.5]` |
| `Add` final bias `b2` | `[1, 1] + [1, 1]` | `[1, 1]` | `[3.25]` | `[-0.25]` |

The zero-input case is a useful check. The first matrix multiply produces zeros, but the first bias gives the second layer a nonzero input. The final answer stays negative. An extra ReLU after the second layer would incorrectly turn `-0.25` into `0`.

## 4. Runtime Setup

I paused the ONNX Runtime installation after the official GitHub Releases page returned a 404 during the session. That did not establish that the release archives were compromised. I did not proceed with an unofficial mirror, and I left the runtime choice open.

I completed the graph design and hand trace without installing a runtime. No `.onnx` model was exported or run as part of this session.

## 5. Next Verification

- [x] Define the graph and tensor shapes.
- [x] Trace both test inputs and record the expected outputs.
- [ ] Create the `.onnx` model with the same weights, biases, and operations.
- [ ] Load it from a standalone C++20 experiment using a CPU runtime.
- [ ] Run both inputs through the same loaded model. Confirm that each output has shape `[1, 1]` and that its value matches the reference within an absolute tolerance of `1e-6`.

## 6. Reflection

The biggest distinction was between a tensor’s values and its shape. Both inputs contain three values and use shape `[3, 1]`, but they produce different intermediate results. Recording those intermediate values gives me a way to narrow down an eventual runtime mismatch instead of guessing from the final number alone.
