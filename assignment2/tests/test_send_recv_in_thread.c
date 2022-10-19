#include <math.h>
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* _______________ | *\_/\*|________ */
/*   |  ___________  |     .-.     .-.      ||_/-\_|______  | */
/*   | |           | |    .****. .****.     | |           | | */
/*   | |   0   0   | |    .*****.*****.     | |   0   0   | | */
/*   | |     -     | |     .*********.      | |     -     | | */
/*   | |   \___/   | |      .*******.       | |   \___/   | | */
/*   | |___     ___| |       .*****.        | |___________| | */
/*   |_____|\_/|_____|        .***.         |_______________| */
/*     _|__|/ \|_|_.............*.............._|________|_ */
/*    / ********** \                          / ********** \ */
/*  /  ************  \                      /  ************  \ */
/* --------------------                    -------------------- */

typedef struct {
  int num;
} seismic_reading;

void *root(void *vargp) {
  /* int send = 99; */
  seismic_reading *my_data;

  my_data = (seismic_reading *) vargp;

  for (int i = 1; i < 4; i++) {
    printf("Sending to %i.\n", i);
    MPI_Send(&( my_data->num ), 1, MPI_INT, i, 0, MPI_COMM_WORLD);
  }
}

void *slave(void *vargp) {
  int recv;
  MPI_Status status;
  while (1) {
    MPI_Recv(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    printf("Received %i from %i.\n", recv, status.MPI_SOURCE);
    sleep(1);
  }
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
    pthread_t thread_id;
    int msg_count;

    seismic_reading to_send;
    to_send.num = 99;

    pthread_create(&thread_id, NULL, root, (void *) &to_send);

  } else {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, slave, NULL);

    int recv;
    MPI_Status status;
  }

  MPI_Finalize();
  return 0;
}
