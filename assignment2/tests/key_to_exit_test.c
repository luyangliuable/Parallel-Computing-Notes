#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *proper_shutdown_master(void *vargp) {
  printf("Let the Battle Begin!\n");
  printf("Press Any Key to Continue\n");
  while (1) {
    char c = getchar();

    if (c == 'q') {
      exit(0);
    }
  }
}

int main(int argc, char *argv[]) {
  pthread_t thread_id;
  pthread_create(&thread_id, NULL, proper_shutdown_master, NULL);

  while (1) {
    printf("running.\n");
    sleep(2);
  }

  return 0;
}
