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
