#include <memory.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

struct timespec start, end, startComp, endComp;

/* clock_gettime(CLOCK_MONOTONIC, &endComp); */

double get_time_taken(struct timespec endComp, struct timespec startComp) {
  double time_taken;
  time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9;
  time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9;
  printf("Cell product complete - Computational time only(s): %lf\n", time_taken);

  return time_taken;
}

/* int main() { */
/*   clock_gettime(CLOCK_MONOTONIC, &startComp); */
/*   sleep(3); */
/*   clock_gettime(CLOCK_MONOTONIC, &endComp); */

  /* double ans = get_time_taken(endComp, startComp); */
/*   printf("%lf\n", ans); */
/* } */
