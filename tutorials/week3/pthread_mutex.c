#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUMTHREADS 4
pthread_mutex_t gMutex;
int g_sum = 0;

void *threadFunc(void *arg) {
  int myVal = rand_r(time(NULL));
  pthread_mutex_lock(&gMutex);
  g_sum += myVal;
  pthread_mutex_unlock(&gMutex);

  return 0;
}

int main() {
  pthread_t hThread[NUMTHREADS];
  pthread_mutex_init(&gMutex, NULL);
  for (int i = 0; i < NUMTHREADS; i++)
    pthread_create(&hThread[i], NULL, threadFunc, NULL);

  for (int i = 0; i < NUMTHREADS; i++)
    pthread_join(hThread[i], NULL);

  printf("Global sum = %f\n", g_sum);
  return 0;
}
