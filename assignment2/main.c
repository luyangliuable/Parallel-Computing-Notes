#include "file_logger.c"
#include "headers.c"
#include "random_readings_generator.c"
#include "log_to_file.c"
#include "utility.c"
#include <math.h>
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

int master_io(MPI_Comm master_comm, MPI_Comm comm, int size);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);
int earthquake_detection_system(int my_rank, int size, MPI_Comm master_comm,
                                MPI_Comm comm, int argc, char **argv);

/* double compute_absolute_diff(double value1, double value2); */
double compute_distance(int *coord1, int *coord2);
void base_station(MPI_Comm master_comm, int *no_of_messages_per_node, struct timespec startComp);

void *proper_shutdown_slave(void *vargp) {
  // TODO NOT WORKING!
  int tmp;
  MPI_Status status;
  MPI_Recv(&tmp, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
  printf("Quiting slave processes.\n");
  exit(0);
}

void *proper_shutdown_master(void *vargp) {
  printf("Press q Key to quit program.\n");

  int size;
  int q = 1;

  while (1) {
    char c = getchar();

    if (c == 'q' || c == 'Q') {
      MPI_Send(&q, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      printf("Quiting.\n");

      MPI_Finalize();
      exit(0);
    }
    sleep(1);
  }
}

int main(int argc, char **argv) {
  /**
   * @brief      The main function the separates the communication into slave
   and master communication through using colors
   *
   * @details    Master has color 1 and slaves have color 0.
   *
   * @param      argc - The argument count
   *
   * @param      argv - The array of string arguments.
   *
   * @return     return 0
   */

  struct timespec start, end, startComp, endComp;

  clock_gettime(CLOCK_MONOTONIC, &startComp);

  int rank, size;
  MPI_Comm new_comm;
  MPI_Init(&argc, &argv);

  /*
   * The size is the number of processes
   * slave = size - 1
   * master = 1
   */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int no_of_messages_per_node[size];
  no_of_messages_per_node[0] = 0;

  /*
   * Split The communication color into two kinds (master, slave)
   * master_color = 1
   * slave_color = 0
   */
  /* MPI_Comm_split(MPI_COMM_WORLD, rank == 0, 0, &new_comm); */
  MPI_Group group;
  MPI_Group old_group;
  MPI_Comm_group(MPI_COMM_WORLD, &old_group);
  int *groups_to_incl = (int *)malloc((size - 1) * sizeof(int));
  for (int i = 1; i < size; i++) {
    groups_to_incl[i - 1] = i;
  }

  MPI_Group_incl(old_group, size - 1, groups_to_incl, &group);
  MPI_Comm_create(MPI_COMM_WORLD, group, &new_comm);

  int size2;

  if (rank != 0) {
    // TODO slave shutdown gives segmentation fault 11
    /* pthread_t thread_id; */
    /* pthread_create(&thread_id, NULL, proper_shutdown_slave, NULL); */

    MPI_Comm_size(new_comm, &size2);
    earthquake_detection_system(rank, size - 1, MPI_COMM_WORLD, new_comm, argc,
                                argv);
  } else {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, proper_shutdown_master, NULL);
    base_station(MPI_COMM_WORLD, no_of_messages_per_node, startComp);
  }

  MPI_Finalize();

  free(groups_to_incl);
  return 0;
}

void base_station(MPI_Comm master_comm, int *no_of_messages_per_node, struct timespec startComp) {
  int no_of_alerts = 0;
  printf("ground node start.\n");
  seismic_reading reading;
  MPI_Datatype MPI_SEISMIC_READING = create_root_datatype(reading);
  MPI_Status status;
  while (1) {
    MPI_Recv(&reading, 1, MPI_SEISMIC_READING, MPI_ANY_SOURCE, 0, master_comm,
             &status);
    no_of_messages_per_node[0]++;
    print_readings(reading);

    no_of_alerts++;
    // TODO compare with seismic balloon sensor
    // If match, conclusive alert
    // Else, inconclusive alert

    struct timespec curr_time;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    double time_taken = (curr_time.tv_sec - startComp.tv_sec) * 1e9;
    time_taken = (time_taken + (curr_time.tv_nsec - startComp.tv_nsec)) * 1e-9;

    log_to_file(time_taken, no_of_alerts, no_of_messages_per_node);
    sleep(3);
  }
}

int earthquake_detection_system(int my_rank, int size, MPI_Comm master_comm,
                                MPI_Comm comm, int argc, char **argv) {
  double absolute_difference[4];
  double euclidean_distances[4];
  int ndims = 2, reorder, my_cart_rank, ierr;
  int nrows, ncols;
  int nbr_i_lo, nbr_i_hi;
  int nbr_j_lo, nbr_j_hi;
  MPI_Comm comm2D;
  int dims[ndims], coord[ndims];
  int wrap_around[ndims];

  /* process command line arguments*/
  if (argc == 3) {
    nrows = atoi(argv[1]);
    ncols = atoi(argv[2]);
    dims[0] = nrows; /* number of rows */
    dims[1] = ncols; /* number of columns */

    if ((nrows * ncols) != size) {
      if (my_rank == 0)
        printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols,
               nrows * ncols, size);
      MPI_Finalize();
      return 0;
    }
  } else {
    nrows = ncols = (int)sqrt(size);
    dims[0] = dims[1] = 0;
  }

  MPI_Dims_create(size, ndims, dims);
  if (my_rank == 1)
    printf("Candidate info showing Rank: %d. Comm Size: %d: Grid Dimension = "
           "[%d x %d] \n",
           my_rank, size, dims[0], dims[1]);

  wrap_around[0] = 0;
  wrap_around[1] = 0; /* periodic shift is .false. */
  reorder = 1;
  ierr = 0;
  ierr = MPI_Cart_create(comm, ndims, dims, wrap_around, reorder, &comm2D);
  if (ierr != 0)
    printf("ERROR[%d] creating CART\n", ierr);

  MPI_Cart_coords(comm2D, my_rank, ndims,
                  coord); // coordinate is returned into the coord array
  /* MPI_Cart_rank(comm2D, coord, &my_cart_rank); */

  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

  int neighbour_ranks[4] = {nbr_i_lo, nbr_i_hi, nbr_j_lo, nbr_j_hi};
  double recv_vals[4];

  seismic_reading seismic_readings[100];
  int c = 0;

  MPI_Request send_request[4];
  MPI_Request receive_request[4];
  MPI_Status send_status[4];
  MPI_Status receive_status[4];

  while (1) {
    double earthquake_magnitude = detect_earthquake(0, 8, my_rank);
    init_reading(&seismic_readings[c], coord[0], coord[1], 9);
    record_current_time(&seismic_readings[c]);
    record_magnitude(&seismic_readings[c], earthquake_magnitude);
    /* print_readings(seismic_readings[c]); */
    MPI_Datatype MPI_SEISMIC_READING =
        create_root_datatype(seismic_readings[c]);
    MPI_Send(&seismic_readings[c], 1, MPI_SEISMIC_READING, 0, 0, master_comm);

    for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
      MPI_Isend(&earthquake_magnitude, 1, MPI_DOUBLE, neighbour_ranks[i], 0,
                comm2D, &send_request[i]);
    }

    recv_vals[0] = -1;
    recv_vals[1] = -1;
    recv_vals[2] = -1;
    recv_vals[3] = -1;

    for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
      MPI_Irecv(&recv_vals[i], 1, MPI_DOUBLE, neighbour_ranks[i], 0, comm2D,
                &receive_request[i]);
    }

    MPI_Waitall(4, send_request, send_status);
    MPI_Waitall(4, receive_request, receive_status);

    for (int i = 0; i < sizeof(neighbour_ranks) / sizeof(int); i++) {
      if (recv_vals[i] == earthquake_magnitude) {
        printf("rank %i and %i have equal magnitude %.2f.\n", my_rank,
               neighbour_ranks[i], earthquake_magnitude);
        log_file(my_rank, neighbour_ranks[i], earthquake_magnitude);
      }
    }

    /***********************************************************************/
    /*                    Calculate absolute differences                   */
    /***********************************************************************/
    for (int i = 0; i < size; i++) {
      int tmp_coord[2];
      if (recv_vals[i] > 0 && i != my_rank) {
        MPI_Cart_coords(
            comm2D, i, ndims,
            tmp_coord); // coordinate is returned into the coord array

        absolute_difference[i] =
            compute_absolute_diff(earthquake_magnitude, recv_vals[i]);

        // Distance in kilometers
        euclidean_distances[i] =
            distance(coord[0], coord[1], tmp_coord[0], tmp_coord[1], 'K');
        printf("(%d, %d): Absolute difference is %.2f.\n", coord[0], coord[1],
               absolute_difference[i]);
        printf("(%d, %d): Distance with (%d, %d) is %.2f.\n", coord[0],
               coord[1], tmp_coord[0], tmp_coord[1], euclidean_distances[i]);
      } else {
        absolute_difference[i] = -1;
        euclidean_distances[i] = -1;
      }
    }

    printf("Global rank: %d. Coord: (%d, %d). Magnitude: %.2f. "
           "Recv Top: %.2f. Recv Bottom: %.2f. Recv Left: %.2f. Recv Right: "
           "%.2f.\n",
           my_rank, coord[0], coord[1], earthquake_magnitude, recv_vals[0],
           recv_vals[1], recv_vals[2], recv_vals[3]);
    sleep(1);
    c++;
  }

  MPI_Comm_free(&comm2D);
  MPI_Finalize();
  return 0;
}
