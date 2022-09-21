#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int numtasks, rank, next, prev, tag1 = 2, tag2 = 2;

  MPI_Request reqs[2];
  MPI_Status stats[2];
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Tasks %i: Started\n", rank);

  if ( rank == 0 ) {
    int test = 4;
    printf("Sending\n");
    sleep(3);
    MPI_Send(&test, 1, MPI_INT, 1, tag1, MPI_COMM_WORLD);
    printf("Sending done\n");
  } else if (rank == 1) {
    for (int i=0; i < 100; i++) {
      sleep(1);
      MPI_Irecv(&next, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[1]);
      printf("%i.\n", next);
      printf("%i.\n", next);
    }
    printf("%i.\n", next);
    printf("%i.\n", next);
    printf("%i.\n", next);
    printf("Recieve done.");
  }

  MPI_Finalize();
  exit(EXIT_SUCCESS);
}
