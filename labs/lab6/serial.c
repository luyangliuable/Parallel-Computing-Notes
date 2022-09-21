#include "check_prime_num.c"
#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "timer.c"
struct timespec start, end, startComp, endComp;

int main(int argc, char ** argv) {
  int size;
  printf("Please enter a number.\n");
  scanf("%i", &size);

  FILE *serial_prime = fopen("serial_prime.txt", "w");

  /* Get the time of clock start *********************************************/
  clock_gettime(CLOCK_MONOTONIC, &startComp);

  for (int i = 0; i < size; i++) {
    if ( check_prime(i) ) fprintf(serial_prime, "%i\n", i);
  }

  fclose(serial_prime);

  /* Get the time ************************************************************/
  clock_gettime(CLOCK_MONOTONIC, &endComp);
  double ans = get_time_taken(endComp, startComp);
  printf("%lf\n", ans);
  return 0;
}
