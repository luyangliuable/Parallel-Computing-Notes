#include <fcntl.h>
#include <memory.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void task1() {
  int size;
  int threads;
  struct timespec start, end, startComp, endComp;


  /***************************************************************************/
  /*                    Get the start time of application                    */
  /***************************************************************************/
  clock_gettime(CLOCK_MONOTONIC, &start);

  /***************************************************************************/
  /*                           Ask for user inputs                           */
  /***************************************************************************/
  printf("Please enter size of the display:\n");
  scanf("%i", &size);

  printf("Please enter the number of threads:\n");
  scanf("%i", &threads);

  /***************************************************************************/
  /*               Set the number of threads according to user               */
  /***************************************************************************/
  omp_set_num_threads(threads);

  /***************************************************************************/
  /*                    Allocate space for random integers                   */
  /***************************************************************************/
  int *rand_int = (int *)calloc(size, sizeof(int));

  printf("size of display: %i\n", size);

  /***************************************************************************/
  /*                  Pointer to the start of rand_int array                 */
  /***************************************************************************/
  int *arr_ptr = &rand_int[0];

  printf("Start parallel processing\n");

  /***************************************************************************/
  /*             Get the clock time for the start of computation             */
  /***************************************************************************/
  clock_gettime(CLOCK_MONOTONIC, &startComp);

#pragma omp parallel for private(size)
  for (int i = 0; i <= size; ++i) {
    srand(time(0));

    *arr_ptr = rand() % 10;
    
    printf("%i", *arr_ptr);
    arr_ptr++;
  }

  /***************************************************************************/
  /*               Get the clock time at the end of computation              */
  /***************************************************************************/
  clock_gettime(CLOCK_MONOTONIC, &endComp);

  double time_taken;

  /***************************************************************************/
  /*                           Calculate time taken                          */
  /***************************************************************************/
  time_taken = (endComp.tv_sec - startComp.tv_sec) * 1e9;
  time_taken = (time_taken + (endComp.tv_nsec - startComp.tv_nsec)) * 1e-9;

  printf("Done, time taken is %lf.\n", time_taken);

  arr_ptr = &rand_int[0];
  
  for (int i = 0; i < sizeof(rand_int)/sizeof(int); i++) {
     printf("%i\n", rand_int[i]);
  }

  /***************************************************************************/
  /*                             Write to display                            */
  /***************************************************************************/
  FILE *fd = fopen("display.txt", "w");

  for (int i = 0; i <= size; ++i) {

    //char *res = (char *)malloc(5 * sizeof(char));

    //snprintf(res, 3 * sizeof(char), "%i\n", *arr_ptr);

    /* printf("%s\n", res); */
    //write(fd, *arr_ptr, 1);
    fprintf(fd, "%d\n", *arr_ptr);
    arr_ptr++;
    //free(res);
  }

  clock_gettime(CLOCK_MONOTONIC, &end);

  fclose(fd);

  free(rand_int);
  /* return 0; */
  printf("finished");
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
    }
  }
  // check occurrences for wins
  for (int i = 0; i < 10; i++) {
    if (digits[i] > 1)
      wins += 1;
    printf("\nDigit: %d Number of occurrences: %d\n", i, digits[i]);
  }
  fp = fopen("wins.txt", "w");
  fprintf(fp, "%d\n", wins);
  printf("Number of wins: %d\n", wins);
  fclose(fp);
  return (0);
}
