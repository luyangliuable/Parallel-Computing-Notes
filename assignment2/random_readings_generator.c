#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

float prev = 0;

double generate_rand(int lo, int hi, int decimal_place, unsigned int *seed) {
  double num = (rand_r(seed) % (hi*decimal_place - lo*decimal_place + 1)) + lo*decimal_place;
  return num;
}

/* int check_prime_number(int n) { */
/*   int i, flag = 1; */

/*   if (n == 0 || n == 1) */
/*     flag = 0; */

/*   for (i = 2; i <= n / 2; ++i) { */
/*     if (n % i == 0) { */
/*       flag = 0; */
/*       break; */
/*     } */
/*   } */

/*   return flag; */
/* } */

float detect_earthquake(int lo, int hi, int rank) {
  sleep(rank);
  unsigned int seed = time(NULL) + rank;
  float random_earthquake_float = generate_rand(lo, hi, 100, &seed)/100;
  /* printf("%.2f, %.2f\n", prev, random_earthquake_float); */
  return random_earthquake_float;
}
