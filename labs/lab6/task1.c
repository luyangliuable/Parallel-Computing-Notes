#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h> /* change to <sys/fcntl.h> for System V */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

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

void close_files(FILE **files) {

  printf("Closing files");
  for (int i = 0; i < sizeof(files) / sizeof(*files); i++) {
    fclose(files[i]);
  }

  printf("Closed all files");
}

int check_prime(int n) {
  int flag = 1, i;
  // Iterate from 2 to sqrt(n)
  for (i = 2; i <= sqrt(n); i++) {
    if (n % i == 0) {
      flag = 0;
      break;
    }
  }

  if (n <= 1)
    flag = 0;

  return flag;
}



int main(int argc, char *argv[]) {
  int rank, s_value, r_value = 1, size, rc, tag1 = 1, tag2 = 2, prev, next;
  FILE *f;

  int buff[2];

  int value_entered = 0;

  MPI_Status Stat;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  FILE ** files = open_files(size);

  printf("Starting.\n");

  prev = rank - 1;
  next = rank + 1;

  if (rank == 0)
    prev = size - 1;
  if (rank == (size - 1))
    next = 0;

  do {
    if (rank == 0) {
      next = rank + 1;
      prev = size - 1;

      if (!value_entered) {
        printf("Enter a round number:\n");
        scanf("%d", &s_value);
        fflush(stdout);
        value_entered = 1;
      } else {
        printf("Task %i: Receiving.\n", rank);
        rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat);
        printf("Task %i: Received from task %i.\n", rank, prev);

        if (r_value < 0) {
          rc = MPI_Send(&r_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
          break;
        };

        s_value = r_value - 1;
      }

      /***********************************************************************/
      /*                 Output to proces_x.txt file section                 */
      /***********************************************************************/

      if (check_prime(r_value)) fprintf(files[rank], "%i\n", r_value);

      fclose(f);

      printf("Task %i: s value is %i. Sending that to task %i.\n", rank,
             s_value, next);
      fflush(stdout);

      // Add your code here
      rc = MPI_Send(&s_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
      fflush(stdout);
    } else {

      // Add your code here
      /* rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat);
       */
      s_value = 0;

      printf("Task %i: Receiving.\n", rank);
      rc = MPI_Recv(&r_value, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &Stat);
      printf("Task %i: Received.\n", rank);

      /***********************************************************************/
      /*                 Output to proces_x.txt file section                 */
      /***********************************************************************/

      if (check_prime(r_value)) fprintf(files[rank], "%i\n", r_value);


      if (r_value < 0) {
        rc = MPI_Send(&r_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
        break;
      };

      s_value = r_value - 1;

      printf("Task %i: s value is %i. Sending that to task %i.\n", rank,
             s_value, next);
      fflush(stdout);

      rc = MPI_Send(&s_value, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
    }
  } while (r_value >= 0);

  printf("End.\n");
  close_files(files);
  MPI_Finalize();

  return 0;
}
