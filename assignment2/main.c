#include "headers.h"
#include "log_to_file.c"
#include "random_readings_generator.c"
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
#define ROOT_RANK 0
#define NO_OF_NEIGHBOURS 4

// Default earthquake threshold;
double EARTHQUAKE_THRESHOLD = 5.5;
double DISTANCE_THRESHOLD = 7500;

/*
 * Argument to put inside struct for thread to see.
 */
typedef struct {
  seismic_reading *reading_ptr;
  MPI_Datatype MPI_SEISMIC_READING;
  MPI_Comm master_comm;
  int *no_of_alerts;
  struct timespec startComp;
  int *count_buffer;
  int size;
} thread_args_base;

/*
 * Argument to put inside struct for thread to see.
 */
typedef struct {
  seismic_reading *reading;
  MPI_Datatype MPI_SEISMIC_READING;
  MPI_Comm comm_2D;
  int neighbour_ranks[NO_OF_NEIGHBOURS];
} thread_args_ground;

double **shared_global_array;

/*****************************************************************************/
/*                            Prototype Functions                            */
/*****************************************************************************/
int earthquake_detection_system(int my_rank, int size, MPI_Comm master_comm,
                                MPI_Comm comm, int *dims, int msg_count,
                                int *count_buffer);
void periodic_detection(int *coord, int ndims, int *dims, int my_rank, int size,
                        int *neighbour_ranks, MPI_Comm comm2D);
void base_station(MPI_Comm master_comm, struct timespec startComp, int size,
                  int msg_count, int *count_buffer);
void get_user_arguments(int size, int rank, int argc, char **argv, int *dims,
                        int *nrows, int *ncols);
double compute_distance(int *coord1, int *coord2);
int master_io(MPI_Comm master_comm, MPI_Comm comm, int size);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);
void *init_threshold_values(int rank);
void log_to_file(double time, int no_of_alerts_detected,
                 seismic_reading reading, int *no_of_messages, int size);

void *balloon_sensor(void *vargp) { exit(0); }

void *proper_shutdown_slave(void *vargp) {
  // TODO NOT WORKING!
  int tmp = 1;
  MPI_Request request;
  MPI_Status status;
  MPI_Irecv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &request);
  MPI_Wait(&request, &status);
  printf("Quiting slave processes.\n");
  MPI_Finalize();
  exit(0);
}

void *proper_shutdown_master(void *vargp) {
  printf("Press q + ENTER to quit program.\n");

  int size;
  int q = 1;

  while (1) {
    char c = getchar();

    if (c == 'q' || c == 'Q') {
      for (int i = 1; i < 8; i++)
        MPI_Send(&q, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

      printf("Quiting.\n");

      MPI_Finalize();
      exit(0);
    }
    sleep(1);
  }
}

void *thread_recv(void *vargs) {
  thread_args_base *params;
params = (thread_args_base *)vargs;
MPI_Status status;

int tmp = 1;

  (params->count_buffer)[0]++;

  MPI_Recv(params->reading_ptr, 1, params->MPI_SEISMIC_READING, MPI_ANY_SOURCE,
           0, MPI_COMM_WORLD, &status);

  ( *(params->reading_ptr) ).source = (char *)get_cur_processs_ip_address();

  printf("At base, source is %s/%i.\n", (*(params->reading_ptr)).source, (*(params->reading_ptr)).source_rank);

  (*(params->no_of_alerts))++;

  print_readings(*(params->reading_ptr));


  /*************************************************************************/
  /*                       Calculate simulation time                       */
  /*************************************************************************/
  struct timespec curr_time;
  clock_gettime(CLOCK_MONOTONIC, &curr_time);

  double time_taken = (curr_time.tv_sec - params->startComp.tv_sec) * 1e9;
  time_taken =
      (time_taken + (curr_time.tv_nsec - params->startComp.tv_nsec)) * 1e-9;

  log_to_file(time_taken, *(params->no_of_alerts), *(params->reading_ptr), params->count_buffer, params->size);

  free(params->reading_ptr);

  return 0;
}

void *thread_send_all(void *vargs) {
  thread_args_ground *params;
  params = (thread_args_ground *)vargs;

  for (int i = 0; i < NO_OF_NEIGHBOURS; i++) {
    MPI_Send((params->reading), 1, params->MPI_SEISMIC_READING,
             (params->neighbour_ranks)[i], 0, params->comm_2D);
    /* (msg_count)++; */
  }

  return 0;
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

  int rank, size;
  MPI_Comm new_comm;
  MPI_Init(NULL, NULL);

  /*
   * The size is the number of processes
   * slave = size - 1
   * master = 1
   */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int *dims = malloc(sizeof(int));
  int nrows, ncols;

  if (rank == 0) {
    pthread_t thread_id;
    double tmp_x;
    double tmp_y;

    pthread_create(&thread_id, NULL, proper_shutdown_master, NULL);

    printf("Please enter the threshold magnitude for earthquake (For default "
           "enter 0):\n");
    scanf("%lf", &tmp_x);
    if (tmp_x) {
      EARTHQUAKE_THRESHOLD = tmp_x;
    }
    printf("New earthquake threshold magnitude is %.2lf.\n",
           EARTHQUAKE_THRESHOLD);

    printf("Please enter the threshold distance for detection (For default "
           "%.2f enter 0):\n",
           DISTANCE_THRESHOLD);
    scanf("%lf", &tmp_y);

    if (tmp_y) {
      DISTANCE_THRESHOLD = tmp_y;
    }

    /* pthread_join(thread_id, NULL); */
  }

  MPI_Barrier(MPI_COMM_WORLD);

  get_user_arguments(size, rank, argc, argv, dims, &nrows, &ncols);

  clock_gettime(CLOCK_MONOTONIC, &startComp);

  /* MPI_Comm_split(MPI_COMM_WORLD, rank == 0, 0, &new_comm); */
  /*
   * Instead of splitting a communicator, choose to create a communicator from
   * scratch
   */
  MPI_Group group;
  MPI_Group old_group;
  MPI_Comm_group(MPI_COMM_WORLD, &old_group);
  int *groups_to_incl = (int *)malloc((size - 1) * sizeof(int));
  for (int i = 1; i < size; i++) {
    groups_to_incl[i - 1] = i;
  }

  MPI_Group_incl(old_group, size - 1, groups_to_incl, &group);

  free(groups_to_incl);

  MPI_Comm_create(MPI_COMM_WORLD, group, &new_comm);

  int cart_size;
  int count_buffer[size];
  int msg_count = 0;
  for (int i = 0; i < size; i++)
    count_buffer[i] = 0;

  if (rank != 0) {
    // TODO slave shutdown gives segmentation fault 11
    /* pthread_t thread_id_slave; */
    /* pthread_create(&thread_id_slave, NULL, proper_shutdown_slave, NULL); */

    MPI_Comm_size(new_comm, &cart_size);

    earthquake_detection_system(rank, size - 1, MPI_COMM_WORLD, new_comm, dims,
                                msg_count, count_buffer);

  } else {
    /*************************************************************************/
    /*                       Root Rank and Base Station */
    /*************************************************************************/

    shared_global_array = malloc(sizeof(double *) * dims[0]);
    *shared_global_array = malloc(sizeof(double) * dims[1]);

    base_station(MPI_COMM_WORLD, startComp, size, msg_count, count_buffer);
  }

  MPI_Finalize();

  free(dims);
  free(groups_to_incl);
  return 0;
}

void base_station(MPI_Comm master_comm, struct timespec startComp, int size,
                  int msg_count, int *count_buffer) {
  int no_of_alerts = 0;
  printf("ground node start.\n");
  seismic_reading reading;

  MPI_Datatype MPI_SEISMIC_READING = create_root_datatype(reading, size);
  MPI_Status status;

  msg_count = 1;
  MPI_Scatter(count_buffer, 1, MPI_INT, &msg_count, 1, MPI_INT, 0,
              MPI_COMM_WORLD);

  while (1) {
    pthread_t thread_id;

    msg_count++;
    MPI_Gather(&msg_count, 1, MPI_INT, count_buffer, 1, MPI_INT, 0,
               MPI_COMM_WORLD);

    thread_args_base thread_args;
    thread_args.MPI_SEISMIC_READING = MPI_SEISMIC_READING;


    // This fixed segmentation fault!!!
    /* ♪┏(・o･)┛♪┗ (･o･) ┓♪ */

    thread_args.reading_ptr = malloc(sizeof(seismic_reading));
    *(thread_args.reading_ptr) = reading;

    thread_args.count_buffer = malloc(sizeof(int) * size);
    thread_args.count_buffer = count_buffer;
    thread_args.master_comm = master_comm;
    thread_args.no_of_alerts = &no_of_alerts;
    thread_args.size = size;
    thread_args.startComp = startComp;

    pthread_create(&thread_id, NULL, thread_recv, (void *)&thread_args);

    pthread_join(thread_id, NULL);
    sleep(2);
  }
}

int earthquake_detection_system(int my_rank, int size, MPI_Comm master_comm,
                                MPI_Comm comm, int *dims, int msg_count,
                                int *count_buffer) {

  int ndims = 2, reorder, ierr;
  int nbr_i_lo, nbr_i_hi;
  int nbr_j_lo, nbr_j_hi;
  MPI_Comm comm2D;
  int coord[ndims];
  int wrap_around[ndims];

  MPI_Scatter(NULL, 1, MPI_INT, &msg_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

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

  /* MPI_Cart_rank(comm2D, coord, &my_cart_rank); */
  /***************************************************************************/
  /*       Update the new rank and coord to match current communicator */
  /***************************************************************************/
  MPI_Comm_rank(comm2D, &my_rank);
  MPI_Cart_coords(comm2D, my_rank, ndims,
                  coord); // coordinate is returned into the coord array

  // Getting neighboring ranks
  MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
  MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

  int neighbour_ranks[NO_OF_NEIGHBOURS] = {nbr_i_lo, nbr_i_hi, nbr_j_lo, nbr_j_hi};

  periodic_detection(coord, ndims, dims, my_rank, size, neighbour_ranks,
                     comm2D);

  MPI_Comm_free(&comm2D);
  MPI_Finalize();
  return 0;
}

void get_user_arguments(int size, int rank, int argc, char **argv, int *dims,
                        int *nrows, int *ncols) {
  if (argc == 3) {
    dims[0] = atoi(argv[1]); /* number of rows */
    dims[1] = atoi(argv[2]); /* number of columns */

    if ((dims[0] * dims[1]) != size - 1) {
      if (rank == 0)
        printf("ERROR: nrows*ncols=%d * %d = %d != %d\n", dims[0], dims[1],
               dims[0] * dims[1], size - 1);
      MPI_Finalize();
      exit(0);
    }
  } else {
    *nrows = *ncols = (int)sqrt(size);
    dims[0] = dims[1] = 0;
  }
}

void periodic_detection(int *coord, int ndims, int *dims, int my_rank, int size,
                        int *neighbour_ranks, MPI_Comm comm2D) {

  seismic_reading recv_vals[NO_OF_NEIGHBOURS];
  seismic_reading seismic_readings[size];
  MPI_Request send_request[NO_OF_NEIGHBOURS];
  MPI_Request receive_request[NO_OF_NEIGHBOURS];
  MPI_Status send_status[NO_OF_NEIGHBOURS];
  MPI_Status receive_status[NO_OF_NEIGHBOURS];

  int msg_count = 0;

  while (1) {
    /*
     * Reset the number of messages each loop.
     * Assume that number of messages for each ground node with neighbor
     * don't change
     */
    unsigned int seed = time(NULL);

    /*
     * Randomly generate readings
     */
    int min = 0.0, max = 9.0;
    double earthquake_magnitude = detect_earthquake(min, max, my_rank);
    double *earthquake_loc = get_earthquake_coord(coord, dims, my_rank);
    double depth = generate_rand(min, max, 2, &seed);

    /*
     * Put the measurements into reading struct.
     * Also store the IP address where the reading is done.
     */
    init_reading(&seismic_readings[my_rank], earthquake_loc[0], earthquake_loc[1], depth, my_rank+1);

    record_current_time(&seismic_readings[my_rank]);

    record_magnitude(&seismic_readings[my_rank], earthquake_magnitude);

    // Used to print_reading here
    MPI_Datatype MPI_SEISMIC_READING =
        create_root_datatype(seismic_readings[my_rank], size);

    // TODO one thread for send one thread for receive?
    /*
     * Send max of 4 messages, 1 with each neighboring node.
     */

    msg_count += NO_OF_NEIGHBOURS;
    /*************************************************************************/
    /*                       Initiated thread arguments */
    /*************************************************************************/
    thread_args_ground thread_arguments;
    thread_arguments.reading = malloc(sizeof(seismic_reading));
    *(thread_arguments.reading) = (seismic_readings[my_rank]);
    thread_arguments.comm_2D = comm2D;

    for (int i = 0; i < NO_OF_NEIGHBOURS; i++) {
      thread_arguments.neighbour_ranks[i] = neighbour_ranks[i];
    }
    thread_arguments.MPI_SEISMIC_READING = MPI_SEISMIC_READING;

    pthread_t send_all_thread_id;
    pthread_create(&send_all_thread_id, NULL, thread_send_all,
                   (void *)&thread_arguments);

    /*************************************************************************/
    /*                         Receive from neighbour                        */
    /*************************************************************************/
    /*
     * Choose to receive on main thread because after finished receiving can
     * proceed with next loop
     */
    for (int i = 0; i < NO_OF_NEIGHBOURS; i++) {
      MPI_Recv(&recv_vals[i], 1, MPI_SEISMIC_READING, neighbour_ranks[i], 0,
               comm2D, &receive_status[i]);
      (msg_count)++;
    }

    /* MPI_Waitall(4, send_request, send_status); */
    /* MPI_Waitall(4, receive_request, receive_status); */

    /*************************************************************************/
    /*                        Compare with neighbors */
    /*************************************************************************/
    int earthquake_detected_flag = 0;

    /*
     * Send max of 4 messages, 1 with each neighboring node.
     */
    for (int i = 0; i < NO_OF_NEIGHBOURS; i++) {
      if (recv_vals[i].magnitude == earthquake_magnitude) {
        printf("rank %i and %i have equal magnitude %.2f.\n", my_rank,
               neighbour_ranks[i], earthquake_magnitude);
      }

      /***********************************************************************/
      /*                    Calculate absolute differences */
      /***********************************************************************/
      if (i != my_rank && neighbour_ranks[i] != -2) {
        int tmp_coord[2];

        // Getting coords from neighboring ranks
        MPI_Cart_coords(comm2D, neighbour_ranks[i], ndims, tmp_coord);

        double dist =
            distance(earthquake_loc[0], earthquake_loc[1],
                     recv_vals[i].latitude, recv_vals[i].longitude, 'K');

        if (earthquake_magnitude > EARTHQUAKE_THRESHOLD &&
            recv_vals[i].magnitude > EARTHQUAKE_THRESHOLD &&
            dist <= DISTANCE_THRESHOLD) {

          if (my_rank < neighbour_ranks[i])
            /*
             * Nominate the lowest rank to send the earthquake message,
             * This is good and simple solution to prevent overhead and u
             * don't need to send 10 messages when u can just send 1 message
             * So in total each ground node sends only max 9 messages to
             * detect an earthquake.
             */
            earthquake_detected_flag = 1;
          else {
            earthquake_detected_flag = 0;
          }
        }
      }
    }

    (msg_count)++;
    if (earthquake_detected_flag)
      (msg_count)++;
    MPI_Gather(&msg_count, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

    if (earthquake_detected_flag) {
      printf("Rank %i, EARTHQUAKE DETECTED with magnitude %.2f.\n", my_rank,
             earthquake_magnitude);

      /*
       * Send only 1 extra message to indicate earthquake
       */
      MPI_Send(&seismic_readings[my_rank], 1, MPI_SEISMIC_READING, 0, 0,
               MPI_COMM_WORLD);
    }

    printf("\rGlobal rank: %d. Coord: (%d, %d). Magnitude: %.2f. "
           "Recv Top: %.2f. Recv Bottom: %.2f. Recv Left: %.2f. Recv Right: "
           "%.2f.\n",
           my_rank, coord[0], coord[1], earthquake_magnitude,
           recv_vals[0].magnitude, recv_vals[1].magnitude,
           recv_vals[2].magnitude, recv_vals[3].magnitude);
    fflush(stdout);
    sleep(2);
  }
}

void *init_threshold_values(int rank) {
  if (rank == 0) {
    pthread_t thread_id;
    double tmp_x;
    double tmp_y;
    pthread_create(&thread_id, NULL, proper_shutdown_master, NULL);
    printf("Please enter the threshold magnitude for earthquake (For default "
           "%.2f enter 0):\n",
           EARTHQUAKE_THRESHOLD);
    scanf("%lf", &tmp_x);
    if (tmp_x) {
      EARTHQUAKE_THRESHOLD = tmp_x;
    }
    printf("New earthquake threshold magnitude is %.2lf.\n",
           EARTHQUAKE_THRESHOLD);

    printf("Please enter the threshold distance for detection (For default "
           "%.2fkm enter 0):\n",
           DISTANCE_THRESHOLD);
    scanf("%lf", &tmp_y);

    if (tmp_y) {
      DISTANCE_THRESHOLD = tmp_y;
    }

    printf("New distance threshold  is %.2lf.\n", DISTANCE_THRESHOLD);
  }

  return 0;
}

void log_to_file(double time, int no_of_alerts_detected,
                 seismic_reading reading, int *no_of_messages, int size) {
  /**
   * @brief      Output the result into logs.txt file
   *
   * @details    Saves the time, no. of alerts detected, seismic reading, and
   * no. of messages into log.
   *
   * @param      time - The simulation time
   *
   * @param      no_of_alerts_detected - the number of alerts detected
   *
   * @param      no_of_messages - the number of messages for each process
   *
   * @param      size - the total number of processes
   */

  char *file_name = "logs.txt";

  int total_no_of_messages = 0;

  /***************************************************************************/
  /*                          Append to output file */
  /***************************************************************************/
  FILE *file = fopen(file_name, "a+");

  fprintf(file, "Simulation Time: %.2f seconds\n", time);
  fprintf(file, "Number of Alerts Detected: %i\n", no_of_alerts_detected);
  fprintf(file, "Number of Messages from base station : %i\n",
          no_of_messages[0]);
  for (int i = 1; i < size; i++) {
    fprintf(file, "Number of Messages from P rank %i with neighbor: %i\n", i,
            no_of_messages[i]);
    total_no_of_messages += no_of_messages[i];
  }
  fprintf(file, "Total Number of Messages: %i\n", total_no_of_messages);
  fprintf(file, "Earthquake source node: %s/%i\n", reading.source,
          reading.source_rank);
  fclose(file);
}
