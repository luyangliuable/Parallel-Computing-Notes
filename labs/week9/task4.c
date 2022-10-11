#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int master_io(MPI_Comm master_comm, MPI_Comm comm, int size);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);

int main(int argc, char **argv) {
  /**
   * @brief      The main function the separates the communication into slave
   and master communication through using colors
   *
   * @details    Master has color 1 and slaves have color 0.
   *
   * @param      argc - The argument count
   *
   * @param      argv - The array of string arguments.
   *
   * @return     return 0
   */

  int rank, size;
  MPI_Comm new_comm;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /*
   * The size is the number of processes
   * slave = size - 1
   * master = 1
   */
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  /*
   * Split The communication color into two kinds (master, slave)
   * master_color = 1
   * slave_color = 0
   */
  /* MPI_Comm_split(MPI_COMM_WORLD, rank == 0, 0, &new_comm); */

  if (rank == 0)
    // Run master
    master_io(MPI_COMM_WORLD, new_comm, size);
  else
    // Run slave processes
    slave_io(MPI_COMM_WORLD, new_comm);

  MPI_Finalize();

  return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm, int size) {
  /**
   * @brief      The master process.
   *
   * @details    Polls for every new message and exit status from slave
   processes.
   *
   * @param      master_comm The communication channel including the subgroup
   new_comm.
   *
   * @param      comm The slave communication channel.
   *
   * @param      size The total number of processes.
   *
   * @return     return 0
   */
  int i, j;
  char buf[256];
  MPI_Status status;
  MPI_Status statuses[size - 1];
  MPI_Request requests[size - 1];

  int finished[size - 1];

  /*
   * flag to indicate that all processes from slave concluded/exited.
   * If flag = 1, exit master because there is not slave nodes left.
   * If flag = 0, keep running master process.
   */
  int flag = 0;
  for (int w = 0; w < size; w++)
    finished[w] = 0;

  while (1) {
    /*
     * Continuously poll for new messages until all slave processes exit.
     */

    printf("-----------------------------\n");
    for (i = 1; i < size; i++) {
      /*
       * Each poll gets the next message from every process.
       */
      if (finished[i - 1] == 0) {
        /*
         * Ignore messages from exited processes even if that is not possible.
         */

        // The get message from any tag. But tag=0 means normal message, tag=1
        // means exit.
        MPI_Recv(buf, 256, MPI_CHAR, i, MPI_ANY_TAG, master_comm, &status);

        if (status.MPI_TAG == 0)
          // Print out normal message
          fputs(buf, stdout);

        if (status.MPI_TAG == 1) {
          // print out status the process exited if it signaled.
          printf("Slave %i exited.\n", i - 1);
          finished[i - 1] = 1;
        }
      }
    }

    /* MPI_Waitall(size-1, requests, statuses); */

    /*************************************************************************/
    /*                If all Processes finished, exit polling                */
    /*************************************************************************/
    flag = 1;
    for (int w = 0; w < size - 1; w++) {
      if (finished[w] == 0)
        flag = 0;
    }

    if (flag == 1)
      break;
  }
  return 0;
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm) {
  /**
   * @brief      The slave process.
   *
   * @details    Possesses a message then send to master process. Upon exit also
   signals to master it is exiting. processes.
   *
   * @param      master_comm The communication channel including the subgroup
   new_comm.
   *
   * @param      comm The slave communication channel.
   *
   * @return     return 0
   */
  char buf[256];
  int rank;
  int exit_tag = 18;
  int master_rank = 0;
  MPI_Request request;

  MPI_Comm_rank(comm, &rank);
  sprintf(buf, "Hello from slave %d\n", rank);
  MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, master_rank, 0, master_comm);

  sprintf(buf, "Goodbye from slave %d\n", rank);
  MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, master_rank, 0, master_comm);

  sprintf(buf, "Hello again from slave %d\n", rank);
  MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, master_rank, 0, master_comm);

  sprintf(buf, "Hello yet again from slave %d\n", rank);
  MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, master_rank, 0, master_comm);

  printf("I'm exiting (%d)\n", rank);

  // Exiting, send to master with a tag of 1.
  MPI_Send(&exit_tag, 1, MPI_INT, master_rank, 1, master_comm);

  return 0;
}
