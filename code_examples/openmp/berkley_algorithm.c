#include <math.h>
#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Status status[size];

  if (rank == 0) {
    int hour = 12;
    int minute = 50;

    int sum = minute;
    for (int i = 1; i < size; i++) {
      int tmp;
      MPI_Recv(&tmp, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status[i]);
      printf("%i.\n", tmp);
      sum += tmp;
    }

    printf("The average time is %.2f.\n", ((double)sum) / ((float)size));

  } else {
    int hour = 12;
    int minute = 50 + rank * 2;
    MPI_Send(&minute, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
