#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

float compute_absolute_diff(float value1, float value2) {
  float res = fabs(value1 - value2);
  return res;
}

float compute_distance(int *coord1, int *coord2) {
  float res =
      sqrt(pow(coord1[1] - coord2[1], 2) + pow(coord1[0] - coord2[0], 2));
  return res;
}

/* int main(int argc, char *argv[]) */
/* { */
/*   int coord1[2] = {1, 1}; */
/*   int coord2[2] = {0, 1}; */
/*   float distance = compute_distance(coord1, coord2); */
/*   printf("(%d, %d): Distance with (%d, %d) is %.2f\n", coord1[0], coord1[1],
 * coord2[0], coord2[1], distance); */
/*   return 0; */
/* } */
