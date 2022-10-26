#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int root(int msg_count, int rank, int *messages) {
  MPI_Scatter(messages, 1, MPI_INT, &msg_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
  printf("Rank %i: %i\n", rank, msg_count);

  msg_count++;
  MPI_Gather(&msg_count, 1, MPI_INT, messages, 1, MPI_INT, 0, MPI_COMM_WORLD);

  printf("Gathered. %i.\n", messages[3]);
}

int slave(int msg_count, int rank) {
  MPI_Scatter(NULL, 1, MPI_INT, &msg_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

  printf("Rank %i: %i\n", rank, msg_count);

  if (rank == 3) {
    msg_count = 999;
  }

  (msg_count)++;
  (msg_count)++;
  MPI_Gather(&msg_count, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
  int messages[8];
  int rank, size;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < 8; i++)
    messages[i] = i;

  if (rank == 0) {
    int msg_count;
    int count_buffer[8];
    root(msg_count, rank, messages);
  } else {
    int msg_count;
    slave(msg_count, rank);
  }

  MPI_Finalize();
  return 0;
}
