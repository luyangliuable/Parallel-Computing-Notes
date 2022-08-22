#include <fcntl.h>
#include <memory.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int task1() {
  int size = 100;
  int threads = 4;
  struct timespec start, end, startComp, endComp;

  /***************************************************************************/
  /*                          Prompt user for inputs */
  /***************************************************************************/

  printf("Please enter size of the display:\n");
  scanf("%i", &size);

  printf("Please enter the number of threads:\n");
  scanf("%i", &threads);

  unsigned int seed = 4;

  FILE *fd = fopen("display.txt", "w");

#pragma omp parallel
  {
    /***************************************************************************/
    /*         Start parallel processing for generating random integer */
    /***************************************************************************/
    printf("Start parallel processing\n");

    int i, random_int;

    /* #pragma omp barrier */

#pragma omp for
    for (i = 0; i < size; i++) {
      /* Generate random number *
       * ************************************************/

      random_int = rand_r(&seed) % 10;

#pragma omp critical
      {
        fprintf(fd, "%i\n", random_int);
      }
    }

    fclose(fd);
  }

  /* free(rand_int); */
  // exit(0);
}

int main() {
  // Read display.txt into numbers array

  task1();

  printf("Start task 2");

  FILE *fp;
  int i;
  fp = fopen("display.txt", "r");
  char cr;
  int lines = 0;
  int wins = 0;
  while (cr != EOF) {
    if (cr == '\n') {
      lines++;
    }
    cr = getc(fp);
  }
  rewind(fp);

  int numbers[lines];
  for (i = 0; i < lines; i++) {
    fscanf(fp, "%d\n", &numbers[i]);
  }

  fclose(fp);

  // Create array to count if digits has already been found
  int digits[10];
  for (int i = 0; i < 10; i++) {
    digits[i] = 0;
  }

  // Check matching entries in parallel
#pragma omp parallel
  {
    int i;
    int my_rank = omp_get_thread_num();
    int num_threads = omp_get_num_threads(); // change to p

    int rpt = lines / num_threads;
    int rptr = lines % num_threads;

    int sp = my_rank * rpt;
    int ep = sp + rpt;

    if (my_rank == num_threads - 1)
      ep += rptr;

    for (int i = sp; i < ep; i++) {
#pragma omp critical
      digits[numbers[i]] += 1;
      printf("Found: %d", numbers[i]);
    }
  }
  // check occurrences for wins
  for (int i = 0; i < 10; i++) {
    if (digits[i] > 1)
      wins += 1;
    printf("Digit: %d Number of occurrences: %d\n", i, digits[i]);
  }

  fp = fopen("wins.txt", "w");
  fprintf(fp, "%d\n", wins);
  printf("Number of wins: %d\n", wins);
  fclose(fp);

  exit(EXIT_SUCCESS);
}
