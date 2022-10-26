#include <omp.h>
#include <stdio.h>

int main()
#pragma omp parallel
{
  int i;
  printf("Hello world");

#pragma omp for
  for (i = 0; i < 6; i++)
    printf("Iter: %d\n", i);

  printf("Goodbye.");
}
