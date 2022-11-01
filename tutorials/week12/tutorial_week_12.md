
## Question 1

What are the components of a GPU in relation to the CUDA programming model? Within a GPU, what is a CUDA core?

* A GPU is composed with blocks, thread, grids and memory. Each grid contains mutlple streaming multiprocessors. Each multiprocess contains multiple steaming processors. Computation is done inside each streaming processor using threads.

* Each streaming processor is also know as a CUDA core.

* The resources are controlled by the programmer through the CUDA programming model.

## Question 2
What are different memories used in a GPU?
Registers
* Local memory
    * Each stream processors uses local memory. All variables declared in kernel (a function executed on GPU) are saved in Local Memory.

* Global memory
    * It is the main memory of GPU. Whenever a memory from GPU is allocated for variables by using cudaMalloc, uses global memory

* Shared memory
    * On one SP, one or more threads can be run. A collection of threads is called a block. Shared memory is shared by all threads in a block.
    * Shared memory is used to reduce latency. This is because global array is very large is search/access time is longer.
    * Latency is memory access delay.
    * Use __shared__ keyword for variable during its declaration.
* Constant memory
    * Is also used to reduce latency.
    * Value access from constant array will be saved cache and will be provided to other threads in half warp.
    * Hence total access will be 1 for all threads accesses.
    * Use __constant__ keyword in variable delaration.

Texture memory
