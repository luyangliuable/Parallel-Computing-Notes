#include "mpi.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *int_to_str(int *integer) {
  char *res;
  int new = *integer;
  int new2 = *integer;

  int l = 0;

  if (*integer == 0) {
    res = malloc(1);
    res = "0";
    return res;
  }

  while (new != 0) {
    new = new / 10;
    l++;
  }

  l++;
  res = malloc(l);

  snprintf(res, l, "%d", *integer);

  return res;
}

FILE **open_files(int size) {
  char *file_n = "process_";

  char **result = malloc(sizeof(char *) * size);
  FILE **files = malloc(sizeof(FILE) * size);

  for (int i = 0; i < size; i++) {
    char *process_id = int_to_str(&i);
    char *file_n2 = malloc(strlen(file_n) + strlen(process_id) + 4);

    strcat(file_n2, file_n);
    strcat(file_n2, process_id);
    strcat(file_n2, ".txt");
    files[i] = fopen(file_n2, "w");
  };

  return files;
}

void close_files(FILE ** files) {
  for(int i = 0; i < sizeof(files)/sizeof(FILE); i++) {
    fclose(files[i]);
  }

  printf("Closed all files");
}

int main(int argc, char ** argv) {
  int size = 4;

  FILE **files = open_files(size);
  fprintf(files[0], "%i\n", 10);
  fprintf(files[1], "%i\n", 123123);
  fprintf(files[2], "%i\n", 12);

  close_files(files);

  return 0;
}
