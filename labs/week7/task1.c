#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *int_to_str(int *integer) {

  if (*integer == 0) {
    return "0";
  }

  int new = *integer;
  int new2 = *integer;

  int l = 0;

  while (new != 0) {
    new = new / 10;
    l++;
  }

  l++;
  char *res = malloc(l);

  snprintf(res, l, "%d", *integer);

  return res;
}

int main(int argc, char **argv) {
  FILE* f;
  int maxn = 10;
  /* double **xlocal = malloc(sizeof(double*) * maxn); */
  /* *xlocal = malloc(sizeof(double) * maxn); */
  double xlocal[maxn][maxn];

  int numtasks=4 + 1, rank, next, prev, tag1 = 1, tag2 = 2;
  int buff[4];

  MPI_Request reqs[numtasks];
  MPI_Status Stat[numtasks];
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);


  /* float *sub_xlocal = malloc(sizeof(double) * maxn); */

  /* int elements_per_proc = maxn / numtasks; */

  /* MPI_Scatter(xlocal, elements_per_proc, MPI_DOUBLE, sub_xlocal, elements_per_proc, MPI_DOUBLE, 0, MPI_COMM_WORLD); */

  if (rank != 4) {

    char *tmp = "process_";
    char *str_rank = int_to_str(&rank);

    char *filename = malloc(strlen(tmp) + strlen(str_rank) + strlen(".txt"));

    strcat(filename, tmp);
    strcat(filename, str_rank);
    strcat(filename, ".txt");

    /* char *t2 = ".txt"; */

    /* char *filename = malloc(strlen(t1) + strlen(t2) + 1); */

    /* filename = strcat(t1, t2); */
    printf("%s.\n", filename);
    f = fopen(filename, "a");

    printf("Currently in rank %i.\n", rank);

    for (int i = 0; i < maxn; i++) {
      xlocal[rank][i] = (double)rank;
      printf("%i ", rank);
      fprintf(f, "%i ", rank);
    }

    printf("\n");
    fprintf(f, "\n");
  }
  /* if ( rank == 0 ) { */
  /*   MPI_Recv(&buff[0], 1, MPI_INT, 1, tag1, MPI_COMM_WORLD, &Stat[0]); */
  /*   printf("%i.\n", buff[0]); */
  /* } else { */
  /*   int send_val = 10; */
  /*   MPI_Isend(&send_val, 1, MPI_INT, 0, tag1, MPI_COMM_WORLD, &reqs[0]); */
  /* } */

  /* MPI_Gather(&sub_xlocal, 1, MPI_DOUBLE, xlocal, 1, MPI_DOUBLE, 0,
   * MPI_COMM_WORLD); */

  MPI_Barrier(MPI_COMM_WORLD);
  /* if (rank == 4) { */
  /*   printf("Done\n"); */
  /* } else { */
  /*   fclose(f); */
  /* }; */

  MPI_Finalize();

  exit(0);
}
