#include <fcntl.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


int main() {
  int size;

  struct timespec start, end, startComp, endComp;

  clock_gettime(CLOCK_MONOTONIC, &start);

  printf("Please enter size of the number of threads to run with:\n");
  scanf("%i", &size);

  /* int *rand_int[size]; */
  int *rand_int = (int *) calloc(size, sizeof(int));

  printf("size of display: %i\n", size);

  srand(time(0));

  int *arr_ptr = &rand_int[0];

  printf("Start parallel processing\n");

  clock_gettime(CLOCK_MONOTONIC, &startComp);

#pragma omp parallel for private(size)
  for (int i = 0; i <= size; ++i) {

    *arr_ptr = rand() % 10;

    arr_ptr++;
  }

  clock_gettime(CLOCK_MONOTONIC, &endComp);

  double time_taken;
  time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9;
  time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9;

  printf("Done, time take is %lf.\n", time_taken);

  arr_ptr = &rand_int[0];

  int fd = open("display.txt", O_WRONLY);
  for (int i = 0; i <= size; ++i) {

    char *res = (char *)malloc(sizeof(char));

    snprintf(res, 3*sizeof(char), "%i\n", *arr_ptr);

    /* printf("%s\n", res); */
    write(fd, res, 2);

    arr_ptr++;
    /* free(res); */
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  close(fd);

  /* free(rand_int); */
  /* return 0; */
  exit(EXIT_SUCCESS);
}
