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
  printf("Please enter size of the number of threads to run with:\n");
  scanf("%i", &size);

  /* int *rand_int[size]; */
  int *rand_int = (int *) calloc(size, sizeof(int));

  printf("size of display: %i\n", size);

  srand(time(0));

  int *arr_ptr = &rand_int[0];

  printf("Start parallel processing\n");

  clock_gettime(CLOCK_MONOTONIC, &start);

#pragma omp parallel for private(size) schedule(static, 4)
  for (int i = 0; i <= size; ++i) {

    *arr_ptr = rand() % 10;

    arr_ptr++;
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  printf("Done, time take is %ld.\n", start.tv_sec  - end.tv_sec);

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

  close(fd);

  /* free(rand_int); */
  /* return 0; */
  exit(EXIT_SUCCESS);
}
