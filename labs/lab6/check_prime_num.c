#include <stdio.h>
#include <math.h>

int check_prime(int n) {
  int flag=1, i;
  // Iterate from 2 to sqrt(n)
  for (i = 2; i <= sqrt(n); i++) {

    // If n is divisible by any number between
    // 2 and n/2, it is not prime
    if (n % i == 0) {
      flag = 0;
      break;
    }
  }

  if (n <= 1) flag = 0;

  return flag;
}

