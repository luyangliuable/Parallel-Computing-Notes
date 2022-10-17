//////////////////////////////////////////////////////////////////////////////////////
// MatrixMul_POSIX_2D_RowPartition.c
// ----------------------------------------------------------------------------------
//
// Multiplies two matrices and writes the resultant multiplication into a text file.
// 2D serial approach. Apply parallel row partitioning using POSIX threads.
//
//
// Last updated date: 9th September 2021 by Vishnu Monn
//////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 4 

int **ppMatrixA = NULL;
int **ppMatrixB = NULL;
unsigned long long **ppMatrixC = NULL;
int rowA = 0;
int colA = 0;
int rowB = 0;
int colB = 0;
int rowC = 0;
int colC = 0;

void* MatrixMulFunc(void *pArg)
{
	int threadId = *((int*)pArg);
	int rowsPerThread, rowsPerThreadRemain;
	int sp, ep;

	if(rowC < NUM_THREADS){
		// Fewer rows than threads
		rowsPerThread = 1;
		rowsPerThreadRemain = 0;
		if(threadId >= rowC)
			return 0;
		sp = threadId * rowsPerThread;
		ep = sp + rowsPerThread;
	}else{
		// Equal or more rows than threads
		rowsPerThread = rowC / NUM_THREADS;
		rowsPerThreadRemain = rowC % NUM_THREADS;

		sp = threadId * rowsPerThread;
		ep = sp + rowsPerThread;
		if(threadId == (NUM_THREADS-1)){
			ep += rowsPerThreadRemain;
		}
	}

	int commonPoint = colA; 
	int i, j, k;
	for(i = sp; i < ep; i++){
		for(j = 0; j < colC; j++){
			for(k = 0; k < commonPoint; k++){
				ppMatrixC[i][j] += (ppMatrixA[i][k] * ppMatrixB[k][j]);
			}
		}
	}

	return NULL;
}

int main()
{
	// Variables
	int i = 0, j = 0;
	pthread_t tid[NUM_THREADS];
	int threadNum[NUM_THREADS];

	/* Clock information */
	struct timespec start, end, startComp, endComp; 
	double time_taken; 
	
	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); 

	// 1. Read Matrix A
	printf("Matrix Multiplication using 2-Dimension Arrays (POSIX - Row Partition) - Start\n\n");

	printf("Reading Matrix A - Start\n");

	FILE *pFileA = fopen("MA.txt", "r");
	fscanf(pFileA, "%d%d", &rowA, &colA);

	ppMatrixA = (int**)malloc(rowA * sizeof(int*));
	for(i = 0; i < rowA; i++){
		ppMatrixA[i] = (int*)malloc(colA * sizeof(int));
	}
	
	for(i = 0; i < rowA; i++){
		for(j = 0; j < colA; j++){
			fscanf(pFileA, "%d", &ppMatrixA[i][j]);
		}

	}
	fclose(pFileA);

	printf("Reading Matrix A - Done\n");

	// 2. Read Matrix B
	printf("Reading Matrix B - Start\n");

	FILE *pFileB = fopen("MB.txt", "r");
	fscanf(pFileB, "%d%d", &rowB, &colB);

	ppMatrixB = (int**)malloc(rowB * sizeof(int*));
	for(i = 0; i < rowB; i++){
		ppMatrixB[i] = (int*)malloc(colB * sizeof(int));
	}

	for(i = 0; i < rowB; i++){
		for(j = 0; j < colB; j++){
			fscanf(pFileB, "%d", &ppMatrixB[i][j]);
		}

	}
	fclose(pFileB);

	printf("Reading Matrix B - Done\n");

	// 3. Perform matrix multiplication 
	printf("Matrix Multiplication - Start\n");

	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &startComp); 

	rowC = rowA;
	colC = colB;
	ppMatrixC = (unsigned long long**)calloc(rowC, sizeof(unsigned long long*));
	for(i = 0; i < rowC; i++){
		ppMatrixC[i] = (unsigned long long*)calloc(colC, sizeof(unsigned long long));
	}

	for (i = 0; i < NUM_THREADS; i++)
	{
	    	threadNum[i] = i;
		pthread_create(&tid[i], 0, MatrixMulFunc, &threadNum[i]);
	}
	
	// Join
	for(i = 0; i < NUM_THREADS; i++)
	{
	    pthread_join(tid[i], NULL);
	}

	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &endComp); 
	time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9; 
    	time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9; 

	printf("Matrix Multiplication - Done\n");
	printf("Matrix Multiplication - Process time (s): %lf\n", time_taken);

	// 4. Write resuls to a new file
	printf("Write Resultant Matrix C to File - Start\n");

	FILE *pFileC = fopen("MC.txt", "w");
	fprintf(pFileC, "%d\t%d\n", rowC, colC);
	for(i = 0; i < rowC; i++){
		for(j = 0; j < colC; j++){
			fprintf(pFileC, "%lld\t", ppMatrixC[i][j]);
		}
		fprintf(pFileC, "\n");
	}
	fclose(pFileC);
	printf("Write Resultant Matrix C to File - Done\n");

	// Clean up
	for(i = 0;  i < rowA; i++){
		free(ppMatrixA[i]);
	}	
	free(ppMatrixA);
	for(i = 0;  i < rowB; i++){
		free(ppMatrixB[i]);
	}
	free(ppMatrixB);	
	for(i = 0;  i < rowC; i++){
		free(ppMatrixC[i]);
	}	
	free(ppMatrixC);

	// Get the clock current time again
	// Subtract end from start to get the CPU time used.
	clock_gettime(CLOCK_MONOTONIC, &end); 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
    	time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 
    	printf("Matrix Multiplication using 2-Dimension Arrays (POSIX - Row Partition) - Done\n");
	printf("Overall time (Including read, multiple and write)(s): %lf\n\n", time_taken);	// ts
	
	return 0;
}
