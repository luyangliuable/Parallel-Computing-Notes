/**
 *   \file sensorio.c
 *   \brief A Documented file.
 *   Completed by Luyang Liu
 *
 */

#include <math.h>
#include <mpi.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define NUM_RANGE 100
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define SHIFT_DEP 2
#define DISP 1
int sensor_io(MPI_Comm world_comm, MPI_Comm comm);
int MeasureTemperature();
bool CheckTemperature(int *recvValues, int temp);
int server_io(MPI_Comm world_comm, MPI_Comm comm);
int main(int argc, char **argv) {
  int rank, size;
  MPI_Comm new_comm;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Comm_split(MPI_COMM_WORLD, rank == size - 1, 0, &new_comm);
  if (rank == size - 1)
    server_io(MPI_COMM_WORLD, new_comm);
  else
    sensor_io(MPI_COMM_WORLD, new_comm);
  MPI_Finalize();
  return 0;
}

int sensor_io(MPI_Comm world_comm, MPI_Comm comm) {
  int ndims = 3, size, my_rank;
  int reorder, my_cart_rank, ierr, worldSize;
  int nbr_i_lo, nbr_i_hi;
  int nbr_j_lo, nbr_j_hi;
  int nbr_k_lo, nbr_k_hi;
  MPI_Comm comm3D;
  int dims[ndims], coord[ndims];
  int wrap_around[ndims];
  char buf[256];

  MPI_Comm_size(world_comm, &worldSize); // size of the world
  /* communicator */
  MPI_Comm_size(comm, &size);    // size of the slave communicator
  MPI_Comm_rank(comm, &my_rank); // rank within the slave communicator
  dims[0] = dims[1] = dims[2] = 0;
  MPI_Dims_create(size, ndims, dims);
  wrap_around[0] = 0;
  wrap_around[1] = 0;
  wrap_around[2] = 0;
  reorder = 1;
  ierr = 0;
  ierr = MPI_Cart_create(comm, ndims, dims, wrap_around, reorder, &comm3D);
  if (ierr != 0)
    printf("ERROR[%d] creating CART\n", ierr);
  MPI_Cart_coords(comm3D, my_rank, ndims, coord);
  MPI_Cart_rank(comm3D, coord, &my_cart_rank);
  MPI_Cart_shift(comm3D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
  MPI_Cart_shift(comm3D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
  MPI_Cart_shift(comm3D, SHIFT_DEP, DISP, &nbr_k_lo, &nbr_k_hi);
  MPI_Request send_request[6];
  MPI_Request receive_request[6];
  MPI_Status send_status[6];
  MPI_Status receive_status[6];
  sleep(my_rank);
  int temp = MeasureTemperature();
  int recvValues[6] = {-1, -1, -1, -1, -1, -1};
  /* INCOMPLETE REGION - START */
  /* COMPLETE PART (g) HERE */

  int neighbours[6] = {nbr_i_lo, nbr_i_hi, nbr_j_lo, nbr_j_hi, nbr_k_lo, nbr_k_hi};

  for (int i = 0; i < 6; i++) {
    MPI_Isend(&temp, 1, MPI_INT, neighbours[i], 0, MPI_COMM_WORLD, &send_request[i]);
  }

  for (int i = 0; i < size; i++) {
      MPI_Irecv(&recvValues[i], 1, MPI_INT, neighbours[i], 0, MPI_COMM_WORLD, &receive_request[i])
  }

  MPI_Waitall(6, send_request, send_status);
  MPI_Waitall(6, receive_request, receive_status);

  /* INCOMPLETE REGION - END */
  if (CheckTemperature(recvValues, temp) == 1) {
    sprintf(
        buf,
        "Fire alert from slave %d at Coord: (%d, %d, %d). Temperature: %d\n",
        my_rank, coord[0], coord[1], coord[2], temp);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, worldSize - 1, 0, world_comm);
  }

  MPI_Comm_free(&comm3D);
  return 0;
}

bool CheckTemperature(int *recvValues, int temp) {
  int retVal = 0;
  for (int i = 0; i < 6; i++) {
    retVal = retVal && (recvValues[i] == temp || recvValues[i] == -1);
  }
  return retVal;
}
int MeasureTemperature() {
  srand(time(NULL));
  int number;
  number = rand() % (NUM_RANGE + 1);
  return number;
}

/* int server_io(MPI_Comm world_comm, MPI_Comm comm) { */
/*   // Not applied to the context of the question */
/* } */
