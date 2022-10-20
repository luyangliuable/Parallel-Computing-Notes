#include "random_readings_generator.c"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

double **glob_array;

typedef struct {
  int dims[2];
} balloon_sensor_args;

void *balloon_sensor(void *vargp) {
  balloon_sensor_args *params = (balloon_sensor_args *)vargp;

  int min = 0.0, max = 9.0;
  int dims[2];
  dims[0] = params->dims[0];
  dims[1] = params->dims[1];
  extern double **glob_array;
  double count = 1;
  double max_count = 20;

  double ***glob_array_ptr = &glob_array;

  *glob_array_ptr = (double **)malloc(sizeof(double *) * dims[0]);
  for (int i = 0; i < dims[1]; i++) {
    *(*glob_array_ptr + i) = (double *)malloc(sizeof(double) * dims[1]);
  }

  while (1) {
    for (int i = 0; i < dims[0]; i++) {
      for (int j = 0; j < dims[1]; j++) {
        glob_array[i][j] = detect_earthquake(min, max, 1);
        /* printf("%.2f.\n", glob_array[i][j]); */
        sleep(1);
      }
    }
    sleep(1);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  pthread_t threadid;

  int dims[2] = {5, 5};

  extern double **glob_array;
  double max_count = 20;
  double ***glob_array_ptr = &glob_array;

  balloon_sensor_args thread_args;
  thread_args.dims[0] = dims[0];
  thread_args.dims[1] = dims[1];

  *glob_array_ptr = (double **)malloc(sizeof(double *) * dims[0]);
  for (int i = 0; i < dims[1]; i++) {
    *(*glob_array_ptr + i) = (double *)malloc(sizeof(double) * dims[1]);
  }

  pthread_create(&threadid, NULL, balloon_sensor, (void *)&thread_args);

  sleep(2);
  for (int i = 0; i < dims[0]; i++) {
    for (int j = 0; j < dims[1]; j++) {
      printf("%.2f.\n", glob_array[i][j]);
      sleep(1);
    }
  }
  return 0;
}
