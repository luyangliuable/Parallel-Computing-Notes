# Week 9 Tutorial

## Question 1

(a)  By comparing Figures 1 and 3, we can observe an approximately 50% improvement in the performance of the serial matrix multiplication algorithm. Explain how the process of swapping lines 90 and 91 in the serial matrix multiplication code file resulted in such a substantial performance improvement.

* Temporal and spatial proximity affects the performance of algorithms.
* In the original for loop, different memory regions are attempted to be access each iteration causing delays.
* If the inner loops are swapped, each iteration will access the same memory region hence faster.

(b) Apart from the optimization as discussed in (a), propose another method which would improve the performance of a serial matrix multiplication algorithm

* Flatten the 2d array into 1d array.
* Every array access would be in the same memory region hence more efficient.

```c
int commonPoint = colA;
for(i = 0; i < rowC; i++){
  for(j = 0; j < colC; j++){
    for(k = 0; k < commonPoint; k++){
    pMatrixC[i * colC + j] += (pMatrixA[i * colA + k] *
    pMatrixB[k * colB + j]);
    }
  }
}
```

## Question 2

```c
int commonPoint = colA;
int i, j, k
#pragma omp parallel for collapse(2) shared(ppMatrixA, ppMatrixB, ppMatrixC, commonPoint, rowC, colC) private(i, j, k) schedule(dynamic)
for(i = 0; i < rowC; i++){
    for(j = 0; j < colC; j++){
        for(k = 0; k < commonPoint; k++){
            ppMatrixC[i][j] += (ppMatrixA[i][k] * ppMatrixB[k][j]);
        }
    }
}
```

## Question 3
* The performance will be better because each process will not need to wait for the data to be passed into the message buffer before moving on. This allows the next iteration to be executed without needing to wait.

## Question 4
