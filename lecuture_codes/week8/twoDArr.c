////////////////////////////////////////////////////////////////////////////
// twoDArr.c
// -------------------------------------------------------------------------
//
// Covers examples on heap 2D arrays
//
// Last updated date: 12th November 2020 by Vishnu Monn
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <string.h>

int main()
{
/*
// 1D array - Stack method (unsafe)
	int i, arrSize;
	printf("Enter 1D array size: ");
	scanf("%d", &arrSize);
	int arr[arrSize]; // Stack based array
	
	for(i = 0;  i < arrSize; i++){
		arr[i] = rand() % 1000;
	}
	
	
	if(arrSize < 100){
		for(i = 0;  i < arrSize; i++){
			printf("%d\t", arr[i]);
		}
	}
	printf("\n");
*/
/*

// 1D array - Heap method (safe)
	int i, arrSize;
	int *pArr;
	
	printf("Enter 1D array size: ");
	scanf("%d", &arrSize);
	pArr = (int*)malloc(arrSize * sizeof(int));
	
	for(i = 0;  i < arrSize; i++){
		pArr[i] = rand() % 1000;
	}
	
	
	if(arrSize < 100){
		for(i = 0;  i < arrSize; i++){
			printf("%d\t", pArr[i]);
		}
	}
	printf("\n");
	free(pArr);
*/

/*
// 2D array - Heap method, flattened 
	int i, j;
	int rows, cols;
	int *pArr;
	
	printf("Enter 2D array size (rows & cols): ");
	scanf("%d%d", &rows, &cols);
	printf("rows: %d & cols: %d\n", rows, cols);
	
	pArr = (int*)malloc((rows * cols) * sizeof(int)); // Flatten the 2D array into a 1 D heap array
	if(pArr != 0){
		printf("Heap array created. Starting address: 0x%x\n", pArr);
	}else{
		printf("Failed to create the heap array. Abort!\n");
		return 0;
	}
	
	printf("Operations\n");
	for(i = 0;  i < rows; i++){
		for(j = 0;  j < cols; j++){
			pArr[(i * cols) + j] = rand() % 1000;
		}
	}
	
	
	if((rows * cols) < 100){
		for(i = 0;  i < rows; i++){
			for(j = 0;  j < cols; j++){
				printf("%d\t", pArr[(i * cols) + j]);
			}
			printf("\n");
		}
	}
	
	printf("Clean up\n");
	printf("\n");
	free(pArr);

*/
// 2D array - Heap method, pointer to pointer 
	int i, j;
	int rows, cols;
	int **pArr;
	
	printf("Enter 2D array size (rows & cols): ");
	scanf("%d%d", &rows, &cols);
	printf("rows: %d & cols: %d\n", rows, cols);
	
	pArr = (int**)malloc(rows * sizeof(int*));
	if(pArr != 0){
		printf("Heap array created. Starting address: 0x%x\n", pArr);
	}else{
		printf("Failed to create the heap array. Abort!\n");
		return 0;
	}

	for(i = 0;  i < rows; i++){
		pArr[i] = (int*)malloc(cols * sizeof(int));
		if(pArr[i] == 0){
			printf("Failed to create the heap array. Abort!\n");
			return 0;
		}
	}
	
	printf("Operations\n");
	for(i = 0;  i < rows; i++){
		for(j = 0;  j < cols; j++){
			pArr[i][j] = rand() % 1000;
		}
	}
	
	
	
	if((rows * cols) < 100){
		for(i = 0;  i < rows; i++){
			for(j = 0;  j < cols; j++){
				printf("%d\t", pArr[i][j]);
			}
			printf("\n");
		}
	}
	printf("\n");
	
	
	printf("Clean up\n");
	for(i = 0;  i < rows; i++)
		free(pArr[i]);	
	free(pArr);
		
	return 0;
}
