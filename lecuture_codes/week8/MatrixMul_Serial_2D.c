//////////////////////////////////////////////////////////////////////////////////////
// MatrixMul_Serial_2D.c
// ----------------------------------------------------------------------------------
//
// Multiplies two matrices and writes the resultant multiplication into a text file.
// 2D serial approach.
//
//
// Last updated date: 9th September 2021 by Vishnu Monn
//////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

int main()
{
	// Variables
	int i = 0, j = 0, k = 0;

	/* Clock information */
	struct timespec start, end, startComp, endComp; 
	double time_taken; 
	
	// Get current clock time.
	clock_gettime(CLOCK_MONOTONIC, &start); 

	// 1. Read Matrix A
	int rowA = 0, colA = 0;

	printf("Matrix Multiplication using 2-Dimension Arrays - Start\n\n");

	printf("Reading Matrix A - Start\n");

	FILE *pFileA = fopen("MA.txt", "r");
	fscanf(pFileA, "%d%d", &rowA, &colA);

	int **ppMatrixA = (int**)malloc(rowA * sizeof(int*));
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
	int rowB = 0, colB = 0;

	printf("Reading Matrix B - Start\n");

	FILE *pFileB = fopen("MB.txt", "r");
	fscanf(pFileB, "%d%d", &rowB, &colB);

	int **ppMatrixB = (int**)malloc(rowB * sizeof(int*));
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

	int rowC = rowA, colC = colB;
	unsigned long long **ppMatrixC = (unsigned long long**)calloc(rowC, sizeof(unsigned long long*));
	for(i = 0; i < rowC; i++){
		ppMatrixC[i] = (unsigned long long*)calloc(colC, sizeof(unsigned long long));
	}

	int commonPoint = colA; 
	for(i = 0; i < rowC; i++){
		for(j = 0; j < colC; j++){
			for(k = 0; k < commonPoint; k++){
				ppMatrixC[i][j] += (ppMatrixA[i][k] * ppMatrixB[k][j]);
			}
		}
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
    	printf("Matrix Multiplication using 2-Dimension Arrays - Done\n");
	printf("Overall time (Including read, multiple and write)(s): %lf\n\n", time_taken);	// ts
	
	return 0;
}
