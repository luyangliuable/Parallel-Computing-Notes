#include "int_to_str.c"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char *log_file(int my_rank, int other_rank, int prime_number) {
  FILE * file;
  char *base_name = "logfile_";
  char *file_suffix = ".txt";
  char *rank = int_to_str(&my_rank);
  /* strcat(file_name, "1"); */
  char * file_name = ( char * ) malloc(strlen(base_name) + strlen(rank) + strlen(file_suffix));
  strcat(file_name, base_name);
  strcat(file_name, rank);
  strcat(file_name, file_suffix);

  file = fopen(file_name, "a+");

  fprintf(file, "rank %i and rank %i have common prime number %i.\n", my_rank, other_rank, prime_number);
  return file_name;
}

/* int main(int argc, char *argv[]) */
/* { */
/*   char *res = log_file(1, 2, 123); */
/*   printf("%s.\n", res); */

/*   exit(0); */
/* } */
