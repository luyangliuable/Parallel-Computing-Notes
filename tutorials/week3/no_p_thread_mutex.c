#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define NUMTHREADS 4
int g_Array[NUMTHREADS];

void *threadFunc(void *arg) {
  int threadNo = *((int *)arg);
  int myVal = rand_r(time(NULL));
  g_Array[threadNo] = myVal;
}

int main() {
  pthread_t hThread[NUMTHREADS];
  int threadNum[NUMTHREADS];
  for (int i = 0; i < NUMTHREADS; i++) {
    threadNum[i] = i;
    pthread_create(&hThread[i], NULL, threadFunc, &threadNum[i]);
  }

  for (int i = 0; i < NUMTHREADS; i++) {
    pthread_join(hThread[i]);
    printf("Thread %d generated value: %d\n", i, g_Array[i]);
  }
}
