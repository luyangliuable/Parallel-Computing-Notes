#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int rank, s_value, r_value=1, size, rc, tag1 = 1, tag2=2, prev, next;

  int buff[2];

  int value_entered = 0;

  MPI_Status Stat;
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  MPI_Comm_size( MPI_COMM_WORLD, &size );

  printf("Starting.\n");

  prev = rank - 1;
  next = rank + 1;

  if (rank == 0) prev = size - 1;
  if (rank == (size-1)) next = 0;

  do {
    if (rank == 0) {
      next = rank + 1;
      prev = size-1;

      if ( !value_entered ) {
        printf("Enter a round number:\n");
        scanf( "%d", &s_value );
        fflush(stdout);
        value_entered = 1;
      } else {
        printf("Task %i: Receiving.\n", rank);
        rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat);
        printf("Task %i: Received from task %i.\n", rank, prev);

        if (r_value < 0) {
          rc = MPI_Send(&r_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
          break;
        };

        s_value = r_value - 1;
      }

      printf("Task %i: s value is %i. Sending that to task %i.\n", rank, s_value, next);
      fflush(stdout);

      // Add your code here
      rc = MPI_Send(&s_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
      fflush(stdout);
    } else {

      // Add your code here
      /* rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat); */
      s_value = 0;

      printf("Task %i: Receiving.\n", rank);
      rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat);
      printf("Task %i: Received.\n", rank);

      if (r_value < 0 ) {
        rc = MPI_Send(&r_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
        break;
      };

      s_value = r_value - 1;

      printf("Task %i: s value is %i. Sending that to task %i.\n", rank, s_value, next);
      fflush(stdout);

      rc = MPI_Send(&s_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);

    }
  } while (r_value >= 0);

  printf("End.\n");
  MPI_Finalize();
}
