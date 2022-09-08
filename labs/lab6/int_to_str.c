#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char *int_to_str(int *integer) {

  int new = *integer;
  int new2 = *integer;

  int l = 0;

  while (new != 0) {
    new = new / 10;
    l++;
  }

  l++;
  char *res = malloc(l);

  snprintf(res, l, "%d", *integer);

  return res;
}


int main() {
  int t = 1000234;
  char t2[] = ".txt";

  char *res = int_to_str(&t);

  char *res2 = malloc(strlen(t2) + strlen(res));

  strcat(res2, res);
  strcat(res2, t2);

  printf("%s\n", res2);


  return 0;
}
