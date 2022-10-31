#include "mpi.h"
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFF_ELEMENTS 1201

int main(int argc, char *argv[]) {
  int my_rank, p;
  int tag = 0, n = 0, i = 0, offset = 0, temp = 0;
  MPI_Status status;
  int *pSendBuffer = 0, *pRecvBuffer = 0;
  int *pPerfectSquare = 0, *pStoreBuff = 0;
  int chunkDistSize, count = 0, totalCount = 0;
  FILE *pFile;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);

  // If my rank is the last rank BUFF_ELEMENTS / p + BUFF_ELEMENTS % p Do the workload plus the remaining
  chunkDistSize = (my_rank == (p - 1)) ? BUFF_ELEMENTS / p + BUFF_ELEMENTS % p : BUFF_ELEMENTS / p;

  pRecvBuffer = (int *)malloc(chunkDistSize * sizeof(int));
  pPerfectSquare = (int *)malloc(chunkDistSize * sizeof(int));

  switch (my_rank) {
  case 0: {
    srand(time(0));
    pSendBuffer = (int *)malloc(BUFF_ELEMENTS * sizeof(int));

    for (i = 0; i < BUFF_ELEMENTS; i++) {
      // Generate a random value
      pSendBuffer[i] = rand() % 1501;
    }
    // Root rank gets a chunck
    memcpy(pRecvBuffer, pSendBuffer, (chunkDistSize * sizeof(int)));

    offset += chunkDistSize;

    for (i = 1; i < p; i++) {
      MPI_Send((int *)pSendBuffer + offset, chunkDistSize, MPI_INT, i, 0, MPI_COMM_WORLD);
      offset += chunkDistSize;
    }
    break;
  }

  default: {
    MPI_Recv(pRecvBuffer, chunkDistSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    break;
  }
  }

  for (i = 0; i < chunkDistSize; i++) {
    temp = (int)sqrt((double)pRecvBuffer[i]);
    if (temp * temp == pRecvBuffer[i]) {
      pPerfectSquare[count] = pRecvBuffer[i];
      count++;
    }
  }

  MPI_Reduce(&count, &totalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  switch (my_rank) {

  case 0: {
    pStoreBuff = (int *)malloc(totalCount * sizeof(int));
    offset = 0;
    memcpy(pStoreBuff, pPerfectSquare, (count * sizeof(int)));
    offset += count;
    for (int i = 1; i < p; i++) {
      MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      MPI_Recv((int *)pStoreBuff + offset, count, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      offset += count;
    }

    pFile = fopen("Perfer_Square_Numbers.txt", "w");
    for (i = 0; i < totalCount; i++) {
      fprintf(pFile, "%d\n", pStoreBuff[i]);
    }
    fclose(pFile);
    free(pStoreBuff);
    free(pSendBuffer);
    break;
  }
  default: {
    MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(pPerfectSquare, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
    break;
  }
  }
  free(pRecvBuffer);
  free(pPerfectSquare);
  MPI_Finalize();
  return 0;
}
