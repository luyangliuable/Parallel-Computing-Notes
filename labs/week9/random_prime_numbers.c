#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int generate_rand(int lo, int hi) {
  srand(time(0));
  int num = (rand() % (hi - lo + 1)) + lo;
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

int generate_prime_number(int lo, int hi) {
  while (1) {
    int a = generate_rand(lo, hi);

    if (check_prime_number(a)) {
      return a;
    };
  }
}

int main(int argc, char **argv) {
  int prime_number = generate_prime_number(0, 100);
  printf("%i.\n", prime_number);
  exit(0);
}
