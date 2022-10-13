//////////////////////////////////////////////////////////////////////////////////////
// MatrixMul_MPI_2D_Submatrix.c
// ----------------------------------------------------------------------------------
//
// Multiplies two matrices and writes the resultant multiplication into a text file.
// 2D parallel approach using MPI with submatrix based partitioning.
// NOTE: This code only works for square matrices (n x n).
// NOTE: This code flattens the 2D arrays into a 1D array.
//
// Last updated date: 12th September 2021 by Vishnu Monn
//////////////////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>
#include <memory.h>
#include <pthread.h>

#define CONV_THREAD_NUM 4


int *pMatrix_A = NULL;
int *pMatrix_B = NULL;
int *pMatrix_B_rotated = NULL;
unsigned long long *pMatrix_C = NULL;
int ma_row = 0, ma_col = 0;
int mb_row = 0, mb_col = 0;
int mc_row = 0, mc_col = 0;

//thread function to rotate Matrix B
void* rotateMatrixFunc(void* pArg){
	int thread_id = *((int*)pArg);
	int rows_per_thread, sp, ep, i, j, remainder;

	rows_per_thread = mb_row / CONV_THREAD_NUM;
	sp = thread_id * rows_per_thread;
	ep = sp + rows_per_thread;
	remainder = mb_row % CONV_THREAD_NUM;
	if(thread_id == CONV_THREAD_NUM-1){
		ep += remainder;
	}

	//Rotate Matrix B
	for(i=sp; i<ep; i++){
		for(j=0; j<mb_col; j++){
			pMatrix_B_rotated[i * mb_col + j] = pMatrix_B[j * mb_col + i];
		}
	}
	return 0;
}

int main(int argc, char* argv[]){
	int my_rank, processors;
	int i=0, j=0, k=0;
	int *pRowBuff = NULL, *pColBuff = NULL;
	unsigned long long *pResBuff = NULL;
	int rows_per_procs, cols_per_procs, element_offset = 0;
	int ref_point, buff_row, buff_col;
	int element_offset_col = 0;
	unsigned long long *pRecvBuff_even = NULL, *pRecvBuff_odd = NULL;
	int curr_row=0, buff_row_index = 0, buff_col_index = 0;
	int rows_remain, cols_remain;
	MPI_Status stat;
	int offset = 0;

	pthread_t matRotateThreads[CONV_THREAD_NUM];
	int matRotateThreadIDs[CONV_THREAD_NUM];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processors);

	if(my_rank == 0){

		printf("Reading Matrix Files - Start\n");

		FILE *pFileA = fopen("MA.txt", "r");
		fscanf(pFileA, "%d%d", &ma_row, &ma_col);

		FILE *pFileB = fopen("MB.txt", "r");
		fscanf(pFileB, "%d%d", &mb_row, &mb_col);

		pMatrix_A = (int*)calloc((ma_row * ma_col), sizeof(int));
		pMatrix_B = (int*)calloc((mb_row * mb_col), sizeof(int));

		offset = 0;
		for(i = 0; i < ma_row; i++){
			for(j = 0; j < ma_col; j++){
				offset = (i * ma_col) + j;
				fscanf(pFileA, "%d", &pMatrix_A[offset]);
			}
		}
		fclose(pFileA);

		offset = 0;
		for(i = 0; i < mb_row; i++){
			for(j = 0; j < mb_col; j++){
				offset = (i * mb_col) + j;
				fscanf(pFileB, "%d", &pMatrix_B[offset]);
			}
		}
		fclose(pFileB);

		printf("Reading Matrix Files - Done\n");


		printf("Matrix Multiplication - Start\n");

		pMatrix_B_rotated = (int*)malloc((mb_row * mb_col) * sizeof(int));

		//Spawn threads to rotate Matrix B
		for(i=0; i<CONV_THREAD_NUM; i++){
			matRotateThreadIDs[i] = i;
			pthread_create(&matRotateThreads[i], 0, rotateMatrixFunc, &matRotateThreadIDs[i]);
		}
		for(i=0; i<CONV_THREAD_NUM; i++){
			pthread_join(matRotateThreads[i], NULL);
		}
	}

	//broadcast row and col number for Matrix A & B to all processors
	MPI_Bcast(&ma_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&ma_col, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&mb_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&mb_col, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if(my_rank == 0){
		rows_per_procs = ma_row / (processors/2);
		rows_remain = ma_row % (processors/2);
		k=0; //index for processors

		//Send rows of Matrix A to all other processors
		for(i=0; i<processors/2; i++){
			if(i != (processors/2)-1){
				for(j=0; j<2; j++){
					if(k==0){
						k++;
						continue;
					}
					else {
						MPI_Send(pMatrix_A + element_offset, rows_per_procs * ma_col, MPI_INT, k, 0, MPI_COMM_WORLD);
						k++;
					}
				}
				element_offset += rows_per_procs * ma_col;
			}else{
				for(j=0; j<2; j++){
						MPI_Send(pMatrix_A + element_offset, (rows_per_procs + rows_remain)  * ma_col, MPI_INT, k, 0, MPI_COMM_WORLD);
						k++;
				}
			}
		}

		cols_per_procs = mb_col / 2;
		cols_remain = mb_col % 2;
		element_offset_col = cols_per_procs * mb_row;

		//Send cols of Matrix B (rows of rotated Matrix B) to all other processors
		for(j=1; j<processors; j+=2){
			MPI_Send(pMatrix_B_rotated + element_offset_col, (cols_per_procs + cols_remain) * mb_row, MPI_INT, j, 0, MPI_COMM_WORLD);
		}
		for(j=2; j<processors; j+=2){
			MPI_Send(pMatrix_B_rotated, cols_per_procs * mb_row, MPI_INT, j, 0, MPI_COMM_WORLD);
		}

	}else{
		mc_row = ma_row;
		mc_col = mb_col;
		rows_per_procs = ma_row / (processors/2);
		cols_per_procs = mb_col /2;
		rows_remain = ma_row % (processors/2);
		cols_remain = mb_col % 2;

		//Receive rows of Matrix A from node 0 for the last two processors
		if(my_rank == processors-1 || my_rank == processors-2){
			pRowBuff = (int*)malloc(((rows_per_procs + rows_remain) * ma_col)  * sizeof(int));
			MPI_Recv(pRowBuff, (rows_per_procs + rows_remain) * ma_col, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}else{ //receive rows from node 0 for other processors
			pRowBuff = (int*)malloc((rows_per_procs * ma_col)  * sizeof(int));
			MPI_Recv(pRowBuff, rows_per_procs * ma_col, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}

		//Receive cols of Matrix B from node 0 for even processors
		if(my_rank % 2 == 0){
			pColBuff = (int*)malloc((cols_per_procs * mb_row) * sizeof(int));
			MPI_Recv(pColBuff, cols_per_procs * mb_row, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}else{ //Receive cols from node 0 for odd processors
			pColBuff = (int*)malloc(((cols_per_procs +cols_remain) * mb_row) * sizeof(int));
			MPI_Recv(pColBuff, (cols_per_procs + cols_remain) * mb_row, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
		}
	}

	if(my_rank == 0){
		mc_row = ma_row; mc_col = mb_col;
		pMatrix_C = (unsigned long long*)calloc((mc_row * mc_col), sizeof(unsigned long long));

		buff_row = ma_row / (processors/2);
		buff_col = mb_col / 2;

		pResBuff = (unsigned long long*)calloc((buff_row * buff_col), sizeof(unsigned long long));

		//Perform matrix multiplication at node 0
		ref_point = ma_col;
		for(i=0; i<buff_row; i++){
			for(j=0; j<buff_col; j++){
				for(k=0; k<ref_point; k++){
					pResBuff[(i*buff_col)+j] += pMatrix_A[(i*ma_col)+k] * pMatrix_B[(k*mb_col)+j];
				}
			}
		}

		//Update results at node 0 to Matrix C
		for(i=curr_row; i<rows_per_procs; i++){
			for(j=0; j<mc_col/2; j++){
				pMatrix_C[(i*mc_col)+j] = pResBuff[(i*buff_col)+j];
			}
		}
		curr_row += rows_per_procs;
	}else{
		buff_row = ma_row / (processors/2);
		buff_col = mb_col / 2;
		ref_point = ma_col;

		//Perform matrix multiplication at even processors
		if(my_rank % 2 == 0){
			if(my_rank == processors -2){ //for second last processors
				pResBuff = (unsigned long long*)calloc(((buff_row + rows_remain) * buff_col), sizeof(unsigned long long));

				for(i=0; i<buff_row + rows_remain; i++){
					for(j=0; j<buff_col; j++){
						for(k=0; k<ref_point; k++){
							pResBuff[(i*buff_col)+j] += pRowBuff[(i*ma_col)+k] * pColBuff[(j*mb_col)+k];
						}
					}
				}

				MPI_Send(pResBuff, (buff_row+rows_remain) * buff_col, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}else{
				pResBuff = (unsigned long long*)calloc((buff_row * buff_col), sizeof(unsigned long long));

				for(i=0; i<buff_row; i++){
					for(j=0; j<buff_col; j++){
						for(k=0; k<ref_point; k++){
							pResBuff[(i*buff_col)+j] += pRowBuff[(i*ma_col)+k] * pColBuff[(j*mb_col)+k];
						}
					}
				}

				MPI_Send(pResBuff, buff_row * buff_col, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}else if(my_rank %2 != 0){ //Perform matrix multiplication at odd processors
			if(my_rank == processors -1){ //for the last processor
				pResBuff = (unsigned long long*)calloc(((buff_row+rows_remain) * (buff_col+cols_remain)), sizeof(unsigned long long));

				for(i=0; i<buff_row+rows_remain; i++){
					for(j=0; j<buff_col + cols_remain; j++){
						for(k=0; k<ref_point; k++){
							pResBuff[(i*(buff_col+cols_remain))+j] += pRowBuff[(i*ma_col)+k] * pColBuff[(j*mb_col)+k];
						}
					}
				}

				MPI_Send(pResBuff, (buff_row+rows_remain) * (buff_col+cols_remain), MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}else{
				pResBuff = (unsigned long long*)calloc((buff_row * (buff_col+cols_remain)), sizeof(unsigned long long));

				for(i=0; i<buff_row; i++){
					for(j=0; j<buff_col + cols_remain; j++){
						for(k=0; k<ref_point; k++){
							pResBuff[(i*(buff_col+cols_remain))+j] += pRowBuff[(i*ma_col)+k] * pColBuff[(j*mb_col)+k];
						}
					}
				}

				MPI_Send(pResBuff, buff_row * (buff_col+cols_remain), MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			}
		}
		free(pColBuff);
		free(pResBuff);
	}

	if(my_rank == 0){
		//Receive results from even processors and save to Matrix C
		for(i=2; i<processors; i+=2){
			if(i != processors-2){
				pRecvBuff_even = (unsigned long long*)malloc((buff_row * buff_col)*sizeof(unsigned long long));
				MPI_Recv(pRecvBuff_even, buff_row * buff_col, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &stat);
				for(j=curr_row; j<rows_per_procs + curr_row; j++){
					for(k=0; k<mc_col/2; k++){
						pMatrix_C[(j*mc_col)+k] = pRecvBuff_even[(buff_row_index*buff_col)+k];
					}
					buff_row_index++;
				}
				curr_row += rows_per_procs;
				buff_row_index = 0;
				free(pRecvBuff_even);
			}else{
				pRecvBuff_even = (unsigned long long*)malloc(((buff_row+rows_remain) * buff_col)*sizeof(unsigned long long));
				MPI_Recv(pRecvBuff_even, (buff_row+rows_remain) * buff_col, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &stat);
				for(j=curr_row; j<rows_per_procs + curr_row + rows_remain; j++){
					for(k=0; k<mc_col/2; k++){
						pMatrix_C[(j*mc_col)+k] = pRecvBuff_even[(buff_row_index*buff_col)+k];
					}
					buff_row_index++;
				}
				buff_row_index = 0;
				free(pRecvBuff_even);
			}
		}

		curr_row = 0;
		buff_row_index = 0;

		//Receive results from odd processors and save to Matrix C
		for(i=1; i<processors; i+=2){
			if(i != processors-1){
				pRecvBuff_odd = (unsigned long long*)malloc((buff_row * (buff_col+cols_remain))*sizeof(unsigned long long));
				MPI_Recv(pRecvBuff_odd, buff_row*(buff_col+cols_remain), MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &stat);

				for(j=curr_row; j<(rows_per_procs + curr_row); j++){
					for(k=mc_col/2; k<mc_col; k++){
						pMatrix_C[(j*mc_col)+k] = pRecvBuff_odd[(buff_row_index*(buff_col+cols_remain))+buff_col_index];
						buff_col_index++;
					}
					buff_row_index++;
					buff_col_index=0;
				}
				curr_row += rows_per_procs;
				buff_row_index = 0;
				buff_col_index = 0;
				free(pRecvBuff_odd);
			}else{
				pRecvBuff_odd = (unsigned long long*)malloc(((buff_row+rows_remain) * (buff_col+cols_remain))*sizeof(unsigned long long));
				MPI_Recv(pRecvBuff_odd, (buff_row+rows_remain)*(buff_col+cols_remain), MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &stat);

				for(j=curr_row; j<(rows_per_procs + curr_row + rows_remain); j++){
					for(k=mc_col/2; k<mc_col; k++){
						pMatrix_C[(j*mc_col)+k] = pRecvBuff_odd[(buff_row_index*(buff_col+cols_remain))+buff_col_index];
						buff_col_index++;
					}
					buff_row_index++;
					buff_col_index=0;
				}
				buff_row_index = 0;
				buff_col_index = 0;
				free(pRecvBuff_odd);
			}
		}

		printf("Matrix Multiplication - Done\n");
	}

	if(my_rank == 0){

		printf("Write Matrix Files - Start\n");

		FILE *pfile_MC = fopen("MC.txt", "w");
		//write the row and column number of Matrix C
		fprintf(pfile_MC, "%d\t%d\n", mc_row, mc_col);

		for(i = 0; i < mc_row; i++){
			for(j = 0; j < mc_col; j++){
				fprintf(pfile_MC, "%lld\t", pMatrix_C[(i * mc_col) + j]);
			}
			fprintf(pfile_MC, "\n");
		}
		fclose(pfile_MC);

		printf("Write Matrix Files - Done\n");

		free(pMatrix_A);
		free(pMatrix_B);
		free(pMatrix_C);
	}
	MPI_Finalize();
	return 0;
}
