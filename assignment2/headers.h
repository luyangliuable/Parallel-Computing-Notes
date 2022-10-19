#include <stdio.h>
#include "ip_address_getter.c"
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "seismic_reading_struct.c"

struct tm *tm;



void print_readings(seismic_reading new_reading) {
  printf("╔════════════════════════════════════════════════════════════════════"
         "════╗"
         "\n");
  printf("║ YYYY ║ MM ║ DD ║ HH ║ MM ║ SS ║ Lat  ║  Long  ║ Magnitude ║ Depth "
         "(km) ║\n");
  printf(
      "║═════════════════════════════════════════════════════════════════════"
      "═══║"
      "\n");
  printf("║ %04d ║ %02d ║ %02d ║ %02d ║ %02d ║ %02d ║%.2f ║ %.2f ║    %.2f   "
         "║    "
         "%.2f   ║\n",
         new_reading.year + 1900, new_reading.month, new_reading.day,
         new_reading.hour, new_reading.minute, new_reading.second,
         new_reading.latitude, new_reading.longitude, new_reading.magnitude,
         new_reading.depth);
  printf(
      "╚═════════════════════════════════════════════════════════════════════"
      "═══╝"
      "\n");
}

int record_current_time(seismic_reading *reading) {
  time_t now = time(0);
  if ((tm = localtime(&now)) == NULL) {
    printf("ERROR: extracting time stuff\n");
    return 1;
  }

  reading->year = tm->tm_year;
  reading->month = tm->tm_mon;
  reading->day = tm->tm_mday;
  reading->hour = tm->tm_hour;
  reading->minute = tm->tm_min;
  reading->second = tm->tm_sec;
  reading->magnitude = 0;

  return 0;
}

int init_reading(seismic_reading *reading, float longitude, float latitude,
                 float depth) {
  reading->longitude = longitude;
  reading->latitude = latitude;
  reading->depth = depth;
  /* reading->source = (char *)get_cur_processs_ip_address(); */

  return 0;
}

int record_magnitude(seismic_reading *reading, float magnitude) {
  reading->magnitude = magnitude;

  return 0;
}

MPI_Datatype create_root_datatype(seismic_reading reading, int size) {
  MPI_Datatype MPI_SEISMIC_READING;

  int no_of_blocks = 12;
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
  array_of_types[9] = MPI_DOUBLE;
  array_of_types[10] = MPI_INT;
  array_of_types[11] = MPI_CHAR;

  MPI_Aint a1, a2;

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
  array_of_blocklengths[9] = 1;
  array_of_blocklengths[10] = size;
  array_of_blocklengths[11] = 1;

  MPI_Aint array_of_displaysments[no_of_blocks];
  MPI_Get_address(&reading, &a1);
  MPI_Get_address(&reading.year, &a2);
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
  MPI_Get_address(&reading.magnitude, &a2);
  array_of_displaysments[8] = a2 - a1;
  MPI_Get_address(&reading.depth, &a2);
  array_of_displaysments[9] = a2 - a1;
  MPI_Get_address(&reading.no_of_messages, &a2);
  array_of_displaysments[10] = a2 - a1;
  MPI_Get_address(&reading.source, &a2);
  array_of_displaysments[11] = a2 - a1;

  MPI_Type_create_struct(no_of_blocks, array_of_blocklengths,
                         array_of_displaysments, array_of_types,
                         &MPI_SEISMIC_READING);

  // Need to commit structure type in MPI
  MPI_Type_commit(&MPI_SEISMIC_READING);

  return MPI_SEISMIC_READING;
}


/* int main(int argc, char *argv[]) { */
/*   int rank, size; */
/*   MPI_Comm new_comm; */
/*   MPI_Init(NULL, NULL); */

/*   MPI_Comm_size(MPI_COMM_WORLD, &size); */
/*   MPI_Comm_rank(MPI_COMM_WORLD, &rank); */

/*   time_t now = time(0); */
/*   if ((tm = localtime(&now)) == NULL) { */
/*     printf("Error extracting time stuff\n"); */
/*     return 1; */
/*   } */

/*   MPI_Status status; */

/*   if (rank == 0) { */
/*     seismic_reading new_reading; */
/*     new_reading.year = tm->tm_year; */
/*     new_reading.month = tm->tm_mon; */
/*     new_reading.day = tm->tm_mday; */
/*     new_reading.hour = tm->tm_hour; */
/*     new_reading.minute = tm->tm_min; */
/*     new_reading.second = tm->tm_sec; */
/*     new_reading.longitude = 512.59; */
/*     new_reading.magnitude = 5.59; */
/*     new_reading.no_of_messages[0] = 59; */
/*     new_reading.no_of_messages[1] = 69; */

/*     MPI_Datatype MPI_SEISMIC_READING = create_root_datatype(new_reading, 3); */

/*     MPI_Send(&new_reading, 1, MPI_SEISMIC_READING, 1, 0, MPI_COMM_WORLD); */

/*   } else if (rank == 1) { */
/*     seismic_reading reading; */
/*     MPI_Datatype MPI_SEISMIC_READING = create_root_datatype(reading, 3); */
/*     MPI_Recv(&reading, 1, MPI_SEISMIC_READING, 0, 0, MPI_COMM_WORLD, &status); */
/*     print_readings(reading); */
/*     printf("%i.\n", reading.no_of_messages[0]); */
/*     printf("%i.\n", reading.no_of_messages[1]); */
/*   } */

/*   MPI_Barrier(MPI_COMM_WORLD); */
/*   MPI_Finalize(); */
/*   exit(0); */
/* } */
