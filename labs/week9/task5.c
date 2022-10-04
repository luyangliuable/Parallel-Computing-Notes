/* Gets the neighbors in a cartesian communicator
 * Orginally written by Mary Thomas
 * - Updated Mar, 2015
 * Link:
 * https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPI-Cart-Comms-and-Topos.pdf
 * Modifications to fix bugs, include an async send and receive and to revise
 * print output
 */
#include "file_logger.c"
#include "random_prime_numbers.c"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define SHIFT_LAYER 2
#define DISP 1

int main(int argc, char *argv[]) {
  /**
   * @brief      3d cartesian virtual topology using mpi
   *
   * @details    A 3d communication cartesian field that sends a random prime
   * number to its immediate neighbour. Then compare the number, if equal write
   * to output file.
   *
   */

  int ndims = 3, size, my_rank, reorder, my_cart_rank, ierr;
  int nrows, ncols, nlayers;
  int nbr_i_lo, nbr_i_hi;
  int nbr_j_lo, nbr_j_hi;
  int nbr_k_lo, nbr_k_hi;
  MPI_Comm comm2D;
  int dims[ndims], coord[ndims];
  int wrap_around[ndims];

  /* start up initial MPI environment */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  /* process command line arguments*/
  if (argc == 4) {
    nrows = atoi(argv[1]);
    ncols = atoi(argv[2]);
    nlayers = atoi(argv[3]);
    dims[0] = nrows;   /* number of rows */
    dims[1] = ncols;   /* number of columns */
    dims[2] = nlayers; /* number of layers */
    if ((nrows * ncols) != size) {
      if (my_rank == 0)
        printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols,
               nrows * ncols, size);
      MPI_Finalize();
      return 0;
    }
  } else {
    dims[0] = dims[1] = dims[2] = 0;
  }

  /*************************************************************/
  /* create cartesian topology for processes */
  /*************************************************************/
  MPI_Dims_create(size, ndims, dims);
  if (my_rank == 0)
    printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d x %d] \n",
           my_rank, size, dims[0], dims[1], dims[2]);

  /* create cartesian mapping */
  wrap_around[0] = 0;
  wrap_around[1] = 0; /* periodic shift is .false. */
  reorder = 1;
  ierr = 0;
  ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder,
                         &comm2D);
  if (ierr != 0)
    printf("ERROR[%d] creating CART\n", ierr);

  int periods[size];

  MPI_Cart_coords(comm2D, my_rank, ndims,
                  coord); // coordinated is returned into the coord array
  MPI_Cart_rank(comm2D, coord, &my_cart_rank);

  /*
   * Get the dimensions for each layer
   */
  MPI_Cart_get(comm2D, ndims, dims, periods, coord);
  nrows = dims[0];
  ncols = dims[1];

  /*
   * The number of layers are the total number of process
   * divided by the number of process on each layer
   */
  nlayers = (int)size / (dims[0] * dims[1]);

  /*
   * Right and left shift by 1
   * get the process 1 block in right and left
   */
  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);

  /*
   * Top bottom shift by 1
   * get the process 1 block in below and above
   */
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

  /*
   * The process in front or below in the 3d cartesian plane
   * is the current rank + 1
   * below is current rank - 1
   */
  if (coord[2] == 1) {
    nbr_k_hi = -2;
  } else {
    nbr_k_hi = my_cart_rank + 1;
  }

  if (coord[2] == 0) {
    nbr_k_lo = -2;
  } else {
    nbr_k_lo = my_cart_rank - 1;
  }

  printf("-------------------\n");
  printf("Current rank is %i. Current coordinates are (%i, %i, %i). Bottom is "
         "%i, top is %i. To the right is %i, to the left is %i. front is "
         "%i, back is %i.\n",
         my_rank, coord[0], coord[1], coord[2], nbr_i_hi, nbr_i_lo, nbr_j_hi,
         nbr_j_lo, nbr_k_hi, nbr_k_lo);
  fflush(stdout);

  MPI_Request send_request[4];
  MPI_Request receive_request[4];
  MPI_Status send_status[4];
  MPI_Status receive_status[4];

  // Generate prime number
  int randomVal = generate_prime_number(0, 10, my_rank);

  int neighbour_ranks[6] = {nbr_i_lo, nbr_i_hi, nbr_j_lo,
                            nbr_j_hi, nbr_k_lo, nbr_k_hi};

  for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
    if (neighbour_ranks >= 0) {
      /*
       * Send to neighboring ranks it's **random prime number**
       */
      MPI_Isend(&randomVal, 1, MPI_INT, neighbour_ranks[i], 0, comm2D,
                &send_request[i]);
    }
  }

  for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
    MPI_Isend(&randomVal, 1, MPI_INT, neighbour_ranks[i], 0, comm2D,
              &send_request[i]);
  }

  int recvValL = -1, recvValR = -1, recvValT = -1, recvValB = -1, recvValF = -1,
      recvValBk = -1;

  int recv_vals[6] = {recvValL, recvValR, recvValT,
                      recvValB, recvValF, recvValBk};

  for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
    MPI_Irecv(&recv_vals[i], 1, MPI_INT, neighbour_ranks[i], 0, comm2D,
              &receive_request[i]);
  }

  /*
   * Wait for all asynchronous requests to finish.
   */
  MPI_Waitall(4, send_request, send_status);
  MPI_Waitall(4, receive_request, receive_status);

  for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
    if (neighbour_ranks[i] >= 0 && recv_vals[i] == randomVal) {
      printf("rank %i and %i have equal prime number %i.\n", my_rank,
             neighbour_ranks[i], randomVal);
      log_file(my_rank, neighbour_ranks[i], randomVal);
    }
  }

  printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d, %d). Random Val: %d. "
         "Recv Top: %d. Recv Bottom: %d. Recv Left: %d. Recv Right: %d. Receive Front: %d. Rev Back: %d.\n",
         my_rank, my_cart_rank, coord[0], coord[1], coord[2], randomVal, recv_vals[0],
         recv_vals[1], recv_vals[2], recv_vals[3], recv_vals[4], recv_vals[5]);
  fflush(stdout);

  MPI_Comm_free(&comm2D);
  MPI_Finalize();
  return 0;
}
