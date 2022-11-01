# Week 2 Tutorial

## 1. Briefly discuss the advantages of using Parallel computing.
* More flexible. Allows each process to work on multiple things such as a client being able to continue with other tasks after sending a request to the server using buffer message approach or early reply.
* More efficient. Allows multiple processes to work a on large task by dividing the task into smaller problem.

## 3. 20% of a program’s execution time is spent within inherently sequential code. What is the limit to the speedup achievable by a parallel version of the program?

S(n) = 1/(.2+(.8)/inf) = 1/(2/10) = 5

## 4. A computer animation program generates a feature movie frame-by-frame. Each frame can be generated independently, and the generated frame is output serially to a file. It takes 99 seconds to render a frame and 1 second to output it. The rendering process of a single frame can be parallelized but writing the frame to file is done serially. If we intend to parallelize the process of rendering a single frame, how much theoretical speedup can be achieved using 100 processors?

s(100) = 100/(1+(100-1)*.01) = 50.2512562814

## 5. Discuss the limitations of Amdahl’s law
* It is an overestimate of a linear speed up and does not take into account the kappa value which is the cost of communication.

## A parallel program executing on 32 processors spends 5% of its time in sequential code. What is the scaled speedup of this program?

S(p) = p + (1-p)ft_s = 32 + (1-32)*.05 = 30.45
