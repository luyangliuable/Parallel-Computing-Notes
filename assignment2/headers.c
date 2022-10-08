#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct tm *tm;

typedef struct {
  struct tm *time;
  int year;        // A string that identifies the process
  int month;       // A string that identifies the process
  int day;         // A string that identifies the process
  int hour;        // A string that identifies the process
  int minute;      // A string that identifies the process
  int second;      // A string that identifies the process
  float latitude;  // The time process entered system
  float longitude; // The time process entered system
  float magnitude; // The total CPU time required by the process int
  float depth;     // Remaining service time until completion
} seismic_reading;

void print_readings(seismic_reading new_reading) {
  printf("╔═════════════════════════════════════════════════════════════════"
         "════╗"
         "\n");
  printf("║ YYYY ║ MM ║ DD ║ HH ║ MM ║ SS ║ Lat ║ Long ║ Magnitude ║ Depth "
         "(km) ║\n");
  printf(
      "║══════════════════════════════════════════════════════════════════"
      "═══║"
      "\n");
  printf("║ %04d ║ %02d ║ %02d ║ %02d ║ %02d ║ %02d ║%.2f ║ %.2f ║    %.2f   ║    "
         "%.2f    ║\n",
         new_reading.year + 1900, new_reading.month, new_reading.day,
         new_reading.hour, new_reading.minute, new_reading.second,
         new_reading.latitude, new_reading.longitude, new_reading.magnitude,
         new_reading.depth);
  printf("╚══════════════════════════════════════════════════════════════════"
         "═══╝"
         "\n");
}

int record_current_time(seismic_reading *reading) {
    time_t now = time(0);
    if ((tm = localtime(&now)) == NULL) {
      printf("ERROR: extracting time stuff\n");
      return 1;
    }

    reading -> time = tm;
    reading->year = tm->tm_year;
    reading->month = tm->tm_mon;
    reading->day = tm->tm_mday;
    reading->hour = tm->tm_hour;
    reading->minute = tm->tm_min;
    reading->second = tm->tm_sec;
    reading->magnitude = 0;

    return 0;
}

int init_reading(seismic_reading *reading, float longitude, float latitude, float depth) {
  reading->longitude = longitude;
  reading->latitude = latitude;
  reading->depth = depth;

  return 0;
}

int record_magnitude(seismic_reading* reading, float magnitude) {
  reading->magnitude = magnitude;

  return 0;
}

/* int main(int argc, char *argv[]) { */
/*   time_t now = time(0); */
/*   if ((tm = localtime(&now)) == NULL) { */
/*     printf("Error extracting time stuff\n"); */
/*     return 1; */
/*   } */

/*   seismic_reading new_reading; */
/*   new_reading.time = tm; */
/*   new_reading.year = tm->tm_year; */
/*   new_reading.month = tm->tm_mon; */
/*   new_reading.day = tm->tm_mday; */
/*   new_reading.hour = tm->tm_hour; */
/*   new_reading.minute = tm->tm_min; */
/*   new_reading.second = tm->tm_sec; */
/*   new_reading.magnitude = 0; */
/*   print_readings(new_reading); */
/*   exit(0); */
/* } */
