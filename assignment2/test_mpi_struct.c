#include "file_logger.c"
/* #include "headers.c" */
#include "random_readings_generator.c"
#include "utility.c"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  int year;         // A string that identifies the process
  int month;        // A string that identifies the process
  int day;          // A string that identifies the process
  int hour;         // A string that identifies the process
  int minute;         // A string that identifies the process
  int second;         // A string that identifies the process
  double latitude;  // The time process entered system
  double longitude; // The time process entered system
  double depth; // The time process entered system
} seismic_reading;

typedef struct {
  int a;
  int b;
  int c;
  double d;
} test;

MPI_Datatype create_root_datatype(seismic_reading reading) {
  MPI_Datatype MPI_SEISMIC_READING;

  int no_of_blocks = 9;
  MPI_Datatype array_of_types[no_of_blocks];
  array_of_types[0] = MPI_INT;
  array_of_types[1] = MPI_INT;
  array_of_types[2] = MPI_INT;
  array_of_types[3] = MPI_INT;
  array_of_types[4] = MPI_INT;
  array_of_types[5] = MPI_INT;
  array_of_types[6] = MPI_DOUBLE;
  array_of_types[7] = MPI_DOUBLE;
  array_of_types[8] = MPI_DOUBLE;

  MPI_Aint a1, a2;

  MPI_Get_address(&reading, &a1);
  MPI_Get_address(&reading.year, &a2);

  int array_of_blocklengths[no_of_blocks];
  array_of_blocklengths[0] = 1;
  array_of_blocklengths[1] = 1;
  array_of_blocklengths[2] = 1;
  array_of_blocklengths[3] = 1;
  array_of_blocklengths[4] = 1;
  array_of_blocklengths[5] = 1;
  array_of_blocklengths[6] = 1;
  array_of_blocklengths[7] = 1;
  array_of_blocklengths[8] = 1;
  MPI_Aint array_of_displaysments[no_of_blocks];
  array_of_displaysments[0] = a2 - a1;
  MPI_Get_address(&reading.month, &a2);
  array_of_displaysments[1] = a2 - a1;
  MPI_Get_address(&reading.day, &a2);
  array_of_displaysments[2] = a2 - a1;
  MPI_Get_address(&reading.hour, &a2);
  array_of_displaysments[3] = a2 - a1;
  MPI_Get_address(&reading.minute, &a2);
  array_of_displaysments[4] = a2 - a1;
  MPI_Get_address(&reading.second, &a2);
  array_of_displaysments[5] = a2 - a1;
  MPI_Get_address(&reading.latitude, &a2);
  array_of_displaysments[6] = a2 - a1;
  MPI_Get_address(&reading.longitude, &a2);
  array_of_displaysments[7] = a2 - a1;
  MPI_Get_address(&reading.depth, &a2);
  array_of_displaysments[8] = a2 - a1;

  MPI_Type_create_struct(no_of_blocks, array_of_blocklengths,
                         array_of_displaysments, array_of_types,
                         &MPI_SEISMIC_READING);

  // Need to commit structure type in MPI
  MPI_Type_commit(&MPI_SEISMIC_READING);

  return MPI_SEISMIC_READING;
}

int main(int argc, char **argv) {

  int rank, size;
  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  seismic_reading reading;
  MPI_Status status[size];

  MPI_Datatype MPI_SEISMIC_READING = create_root_datatype(reading);

  if (rank == 0) {
    printf("rank1.\n");
    reading.year = 1997;
    reading.month = 12;
    reading.day = 25;
    reading.hour = 12;
    reading.minute = 56;
    reading.latitude = 23.123123;
    reading.longitude = 3.123;
    reading.depth = 1221.2;
    MPI_Send(&reading, 1, MPI_SEISMIC_READING, 1, 0, MPI_COMM_WORLD);
    /* printf("Rank %i: year %i, day %i, depth %.2f.\n", rank, reading.year, */
    /*        reading.day, reading.depth); */
  } else {
    seismic_reading reading2;
    MPI_Recv(&reading2, 1, MPI_SEISMIC_READING, 0, 0, MPI_COMM_WORLD,
             &status[rank]);
    /* print_readings(reading); */
    printf("Rank %i: year %i, month %i, day %i, hour %i, lat %f, long %f, depth %f.\n",
           rank, reading2.year, reading2.month, reading2.day, reading2.hour,
           reading2.latitude, reading2.longitude, reading2.depth);
  }

  MPI_Finalize();
  return 0;
}
