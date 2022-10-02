#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int generate_rand(int lo, int hi, unsigned int * seed) {
  int num = (rand_r(seed) % (hi - lo + 1)) + lo;
  return num;
}

int check_prime_number(int n) {
  int i, flag = 1;

  if (n == 0 || n == 1)
    flag = 0;

  for (i = 2; i <= n / 2; ++i) {
    if (n % i == 0) {
      flag = 0;
      break;
    }
  }

  return flag;
}

int generate_prime_number(int lo, int hi, int rank) {
  sleep(rank);
  unsigned int seed = time(NULL);

  while (1) {
    int a = generate_rand(lo, hi, &seed);

    if (check_prime_number(a)) {
      return a;
    };
  }
}

/* int main(int argc, char **argv) { */
/*   /\* unsigned int seed = time(NULL); *\/ */
/*   /\* generate_rand(0, 100, &seed); *\/ */
/*   int prime_number = generate_prime_number(0, 100, 1); */
/*   printf("%i.\n", prime_number); */
/*   exit(0); */
/* } */
