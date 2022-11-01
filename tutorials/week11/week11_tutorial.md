# SIMD (Single Instruction Multiple Data)

## Question 1
See notes


## SSE2 New Features

* SSE2 adds new math instructions for double-precision floating point and also extends MMX instructions to operate on 128-bit XMM registers.

* Allows programmer to perform math on any datatype (from 8bit integer to 64-bit float) with the XMM registers not the legacy MMX/FPU registers.

## SSE3 New Features
* 13 new instructions were introduced with SSE3.
* New capability to work horizontally in a register as opposed to the more or less strictly vertical option of all previous SSE instructions.
* Asymmetrical processing so that different operation on SIMD data can be performd.

## List key improvements of AVX over SSE4.
* AVX added larger registers 256-bit floating point registers and widened data path from 128bits to 256 bits and 3 operand instructions

## Difference between features of MMX, SSE, SSE2, SSE3, SS4 and AVX.
| 1997                                          | 1999                                                                   | 2000                                                                                       | 2003                   | 2007                                          | 2011                                   |
|:---------------------------------------------:|:----------------------------------------------------------------------:|:------------------------------------------------------------------------------------------:|:----------------------:|:---------------------------------------------:|:--------------------------------------:|
| MMX                                           | SSE                                                                    | SSE2                                                                                       | SSE3                   | SSE4                                          | AVX                                    |
| 8 64-bit registers                            | 8 128 bit registers expandable to 16 registers for 64 bit architecture | Enabled both integer and floating based operations to be performed using the XMM registers | Horizontal operations. | Additional instructions for vector operations | 16 256 bit registers                   |
| MM0 -> MM7                                    | XMM0 -> XMM8, to XMM15                                                 |                                                                                            | Asymmetric processing  |                                               | YMM0 -> YMM15                          |
| Only performs integer based vector operations | Performs floating point based vector operations                        |                                                                                            |                        |                                               | New instructions for vector operations |
| Shares registers with x87 FPU                 |                                                                        |                                                                                            |                        |                                               |                                        |
| Cannot perform integer and FPU                |                                                                        |                                                                                            |                        |                                               |                                        |

