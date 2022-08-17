# Tutorial 2
<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [Tutorial 2](#tutorial-2)
    - [Questions](#questions)
        - [Advantages of using Parallel Computing](#advantages-of-using-parallel-computing)
        - [Disadvantages of using Parallel Computing](#disadvantages-of-using-parallel-computing)
    - [Estimate the maximum theoretical dual precision performance of your personal laptop](#estimate-the-maximum-theoretical-dual-precision-performance-of-your-personal-laptop)
    - [Percentage Speedup](#percentage-speedup)
    - [Limitations of Amdahl's Law](#limitations-of-amdahls-law)

<!-- markdown-toc end -->

## Questions
### Advantages of using Parallel Computing
Minimises all parts of the hardware being idle and can run multiple instructions at the same time.

* Saves time
* Can solve larger problems
* Can do more work if you split the work in different computer.

### Disadvantages of using Parallel Computing
* Programmer responsibility for synchronisation constructs between memory and CPU.
* It becomes increasingly difficult and expensive to design and produce shared memory machines with ever increasing numbers of processors.


## Estimate the maximum theoretical dual precision performance of your personal laptop

* P = N × C × F × R

N = 4
C = 8
F = ?
R = 2064 - 3200 MHz

P = 4*8*2.6*3200*10^3 = 2.6 TeraFlops

## Percentage Speedup
s = 1/(0.8 + (1-0.8)/0.2) = 55%

## Limitations of Amdahl's Law
* Physical limitations include overheating and bottle necking which the equation does not consider.
* It is an upper limit and you may not achieve the performance due to software

Elbowing out effect?

## A parallel program executing on 32 processors spends 5% of its time in sequential code. What is the scaled speedup of this program?

p = 32
s = 1/(0.05 + (1-0.05)/32) = 12.5490196078% speedup

