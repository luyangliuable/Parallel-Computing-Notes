//////////////////////////////////////////////////////////////////////////////////////
// MatrixMul_MPI_2D.c
// ----------------------------------------------------------------------------------
//
// Multiplies two matrices and writes the resultant multiplication into a text
// file. 2D parallel approach using MPI with row based partitioning.
//
//
// Last updated date: 9th September 2021 by Vishnu Monn
//////////////////////////////////////////////////////////////////////////////////////
#include "mpi.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int my_rank, processors;
  int i = 0, j = 0, k = 0;
  int rows_per_procs, row_offset = 0;
  int rows_remain, commonPoint;
  MPI_Status stat;

  int **ppMatrixA = NULL;
  int **ppMatrixB = NULL;
  unsigned long long **ppMatrixC = NULL;
  int ma_row = 0, ma_col = 0;
  int mb_row = 0, mb_col = 0;
  int mc_row = 0, mc_col = 0;
  double startTime, endTime;
  int c_i = 0, c_j = 0;
  int startRow, endRow;
  int rowsToSendOrReceive, loopCondition;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &processors);

  // 1. Root process reads the content of matrices A and B from file
  if (my_rank == 0) {
    printf(
        "Parallel Matrix Multiplication using 2-Dimension Arrays - Start\n\n");

    printf("Reading Matrix A - Start\n");

    FILE *pFileA = fopen("MA.txt", "r");
    fscanf(pFileA, "%d%d", &ma_row, &ma_col);

    ppMatrixA = (int **)malloc(ma_row * sizeof(int *));
    for (i = 0; i < ma_row; i++) {
      ppMatrixA[i] = (int *)malloc(ma_col * sizeof(int));
    }

    for (i = 0; i < ma_row; i++) {
      for (j = 0; j < ma_col; j++) {
        fscanf(pFileA, "%d", &ppMatrixA[i][j]);
      }
    }
    fclose(pFileA);

    printf("Reading Matrix A - Done\n");

    // 2. Read Matrix B
    printf("Reading Matrix B - Start\n");

    FILE *pFileB = fopen("MB.txt", "r");
    fscanf(pFileB, "%d%d", &mb_row, &mb_col);

    ppMatrixB = (int **)malloc(mb_row * sizeof(int *));
    for (i = 0; i < mb_row; i++) {
      ppMatrixB[i] = (int *)malloc(mb_col * sizeof(int));
    }

    for (i = 0; i < mb_row; i++) {
      for (j = 0; j < mb_col; j++) {
        fscanf(pFileB, "%d", &ppMatrixB[i][j]);
      }
    }
    fclose(pFileB);

    printf("Reading Matrix B - Done\n");

    // Start the computation time (which covers the communication time)
    startTime = MPI_Wtime();
  }

  // 2. Root process broadcast the row and col number for Matrix A & B to all
  // processes
  //  Each process computes the region of the matrix to work on
  MPI_Bcast(&ma_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&ma_col, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&mb_row, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&mb_col, 1, MPI_INT, 0, MPI_COMM_WORLD);

  rows_per_procs = ma_row / processors;
  rows_remain = ma_row % processors;

  startRow = my_rank * rows_per_procs;
  endRow = startRow + rows_per_procs;
  if (my_rank == processors - 1)
    endRow += rows_remain;

  if (my_rank == 0) {
    mc_row = ma_row;
    mc_col = mb_col;
  } else {
    mc_row = endRow - startRow;
    mc_col = mb_col;
    ma_row = endRow - startRow;
  }
  commonPoint = ma_col;

  if (my_rank != 0) {
    ppMatrixA = (int **)malloc(ma_row * sizeof(int *));
    for (i = 0; i < ma_row; i++) {
      ppMatrixA[i] = (int *)malloc(ma_col * sizeof(int));
    }

    ppMatrixB = (int **)malloc(mb_row * sizeof(int *));
    for (i = 0; i < mb_row; i++) {
      ppMatrixB[i] = (int *)malloc(mb_col * sizeof(int));
    }
  }

  ppMatrixC =
      (unsigned long long **)calloc(mc_row, sizeof(unsigned long long *));
  for (i = 0; i < mc_row; i++) {
    ppMatrixC[i] =
        (unsigned long long *)calloc(mc_col, sizeof(unsigned long long));
  }

  // 3. Root process distribute matrices A and B to other processes

  MPI_Status status[100];
  MPI_Request request[100];
  int messageCounter = 0;
  if (my_rank == 0) {
    row_offset = rows_per_procs;
    for (i = 1; i < processors; i++) {
      if (i == processors - 1) {
        loopCondition = (i * rows_per_procs) + rows_per_procs + rows_remain;
      } else {
        loopCondition = (i * rows_per_procs) + rows_per_procs;
      }

      for (j = row_offset; j < loopCondition; j++) {
        /* MPI_Send(ppMatrixA[j], ma_col, MPI_INT, i, 0, MPI_COMM_WORLD); */
        MPI_Isend(ppMatrixA[j], ma_col, MPI_INT, i, 0, MPI_COMM_WORLD, &request[messageCounter++]);
      }
      row_offset += rows_per_procs;
    }
  } else {
    rowsToSendOrReceive = endRow - startRow;
    for (i = 0; i < rowsToSendOrReceive; i++) {
      /* MPI_Recv(ppMatrixA[i], ma_col, MPI_INT, 0, 0, MPI_COMM_WORLD, &stat); */
      MPI_Irecv(ppMatrixA[i], ma_col, MPI_INT, 0, 0, MPI_COMM_WORLD, &request[messageCounter++]);
    }
  }
  MPI_Waitall(messageCounter, request, status);

  for (i = 0; i < mb_row; i++) {
    MPI_Bcast(ppMatrixB[i], mb_col, MPI_INT, 0, MPI_COMM_WORLD);
  }

  // 4. Each process performs the matrix multiplication - parallel computation
  for (i = 0, c_i = 0; i < ((my_rank == processors - 1) ? (rows_per_procs + rows_remain) : rows_per_procs); i++, c_i++) {
    for (j = 0, c_j = 0; j < mc_col; j++, c_j++) {
      for (k = 0; k < commonPoint; k++) {
        ppMatrixC[c_i][c_j] += ppMatrixA[i][k] * ppMatrixB[k][j];
      }
    }
  }

  // 5. The root process receives results of the matrix multiplication from
  // other processes to be combined into a single array
  if (my_rank == 0) {

    row_offset = rows_per_procs;
    for (i = 1; i < processors; i++) {
      if (i == processors - 1) {
        loopCondition = (i * rows_per_procs) + rows_per_procs + rows_remain;
      } else {
        loopCondition = (i * rows_per_procs) + rows_per_procs;
      }
      for (j = row_offset; j < loopCondition; j++) {
        /* MPI_Recv(ppMatrixC[j], mc_col, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &stat); */
        MPI_Irecv(ppMatrixC[j], mc_col, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &request[my_rank]);
      }
      row_offset += rows_per_procs;
    }

  } else {
    rowsToSendOrReceive = endRow - startRow;
    for (i = 0; i < rowsToSendOrReceive; i++) {
      /* MPI_Send(ppMatrixC[i], mc_col, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD); */
      MPI_Isend(ppMatrixC[i], mc_col, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &request[my_rank]);
    }
  }
  MPI_Waitall(processors, request, status);

  // 6. The root process writes the result of the matrix multiplication to a
  // file.
  if (my_rank == 0) {
    printf("Matrix Multiplication - Done\n");
    endTime = MPI_Wtime();
    printf("Matrix Multiplication - End. Process time (s): %lf\n",
           endTime - startTime);

    // Write the results of Matrix Multiplication into file
    FILE *pfile_MC = fopen("MC.txt", "w");
    // write the row and column number of Matrix C
    fprintf(pfile_MC, "%d\t%d\n", mc_row, mc_col);

    for (i = 0; i < mc_row; i++) {
      for (j = 0; j < mc_col; j++) {
        fprintf(pfile_MC, "%lld\t", ppMatrixC[i][j]);
      }
      fprintf(pfile_MC, "\n");
    }
    fclose(pfile_MC);

    printf("Write Matrix Files - Done\n");
  }

  // Clean up
  for (i = 0; i < ma_row; i++) {
    free(ppMatrixA[i]);
  }
  free(ppMatrixA);
  for (i = 0; i < mb_row; i++) {
    free(ppMatrixB[i]);
  }
  free(ppMatrixB);
  for (i = 0; i < mc_row; i++) {
    free(ppMatrixC[i]);
  }
  free(ppMatrixC);

  MPI_Finalize();
  return 0;
}
