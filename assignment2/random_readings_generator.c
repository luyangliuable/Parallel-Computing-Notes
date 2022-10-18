#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

float prev = 0;

double generate_rand(double lo, double hi, int decimal_place, unsigned int *seed) {
  int precision = 100000;
  int low = ( int )( lo * precision );
  int high = ( int )( hi * precision );
  /* int was_neg = 0; */

  /* if ( high < 0 && low < 0) { */
  /*   was_neg = 1; */
  /*   high = -high; */
  /*   low = -low; */
  /* } */

  if (high < low) {
    double tmp = low;
    low = high;
    high = tmp;
  }

  double num = (rand_r(seed) % (high - low  + 1)) + low;

  num = num / precision;

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

double detect_earthquake(double lo, double hi, int rank) {
  sleep(rank);
  unsigned int seed = time(NULL) + rank;
  double random_earthquake_float = generate_rand(lo, hi, 100, &seed);
  /* printf("%.2f, %.2f\n", prev, random_earthquake_float); */
  return random_earthquake_float;
}

double *get_earthquake_coord(int *coord, int *dimension, int rank) {
  /*
   * Boundaries of Victoria and coordinates of detections will be detected in
   * quadrants of this boundary
   */
  unsigned int seed = time(NULL) + rank;
  /* sleep(rank); */
  double boundary[4][2] = {{-30.42131084079751, 140.9994933895501},
                           {-30.615331183290877, 152.87990604663332},
                           {-37.82272667427901, 139.50740364825768},
                           {-37.77823707316231, 150.03648974718925}};

  double min_lon = boundary[2][1];
  double max_lon = boundary[2][1] +
                   coord[0] * (boundary[1][1] - boundary[2][1]) / dimension[0];
  double min_lat = boundary[2][0];
  double max_lat = boundary[0][0] + (coord[1]) *
                                        (boundary[2][0] - boundary[0][0]) /
                                        dimension[1];

  double random_lat = generate_rand(min_lat, max_lat, 100, &seed);
  double random_lon = generate_rand(min_lon, max_lon, 100, &seed);

  double *res = malloc(sizeof(double) * 2);
  res[0] = random_lat;
  res[1] = random_lon;

  return res;
}

/* int main(int argc, char *argv[]) { */
/*   int coord[2] = {1, 0}; */
/*   int dimension[2] = {7, 1}; */
/*   double *detection_loc = get_earthquake_coord(coord, dimension, 4); */
/*   printf("Random lat %.6f. Random lon %.6f.\n", detection_loc[0], detection_loc[1]); */
/*   return 0; */
/* } */
