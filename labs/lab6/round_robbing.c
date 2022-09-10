#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int numtasks, rank, next, prev, tag1=1, tag2=2;

  MPI_Request reqs[4];
  MPI_Status stats[4];
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Tasks %i: Started\n", rank);

  prev = rank - 1;
  next = rank + 1;

  if (rank == 0) prev = numtasks - 1;

  if (rank == (numtasks-1)) next = 0;

  int buff[2];

  if (rank != 0) MPI_Wait(&reqs[prev], &stats[prev]);

  MPI_Irecv(&buff[0], 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &reqs[0]);
  MPI_Irecv(&buff[1], 1, MPI_INT, next, tag2, MPI_COMM_WORLD, &reqs[1]);

  printf("Task %i: received %i from previous task %i.\n",rank, buff[0], prev);
  printf("Task %i: received %i from next task %i.\n", rank, buff[1], prev);

  MPI_Isend(&rank, 1, MPI_INT, prev, tag2, MPI_COMM_WORLD, &reqs[2]);
  MPI_Isend(&rank, 1, MPI_INT, next, tag1, MPI_COMM_WORLD, &reqs[3]);

  printf("Task %i: sending %i to next task %i.\n", rank, rank, next);
  /* MPI_Wait(&reqs[rank], &stats[rank]); */
  int a = 1;
  printf("Task %i: Status is %s.\n", MPI_Request_get_status(reqs[rank], &a, &stats[rank]));
  MPI_Waitall(4, reqs, stats);

  MPI_Finalize();

  return 0;
}
