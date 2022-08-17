#include <stdio.h>
/* #include <omp.h> */
#include<stdlib.h>
#include <math.h>

/* Function prototypes
 *******************************************************/
void WriteToFile(char *pFilename, int *pMatrix, int inRow, int inCol);


int checkPrime(int n) {
  int i, flag = 0;
  // 0 and 1 are not prime numbers
  // change flag to 1 for non-prime number
  if (n == 0 || n == 1)
    flag = 1;

  for (i = 2; i <= n / 2; ++i) {

    // if n is divisible by i, then n is not prime
    // change flag to 1 for non-prime number
    if (n % i == 0) {
      /* printf("%d is not a prime\n", n); */
      flag = 1;
      break;
    }
  }

  return flag;
}


int main() {
/**
 * @brief      Main function
 *
 * @details    Asks the user for a prime number and does a bit of parallel processing then returns the result.
 *
 * @param      none
 *
 * @return     return type int
 */

  int number;
  int prime_numbers[] = {1, 3, 5, 7, 11};

  /* Function pointer to files
  *****************************************************************************/
  FILE *fptr;

  fptr = fopen("output.txt", "a");

  printf("Please enter a prime number\n");
  scanf("%d", &number);
  printf("user inputted %d\n", number);

  /* int MAX = 100; */

  char output[] = "output.txt";
  # pragma omp parallel
  {
    for (int i = 0; i < number; i++) {
      if (checkPrime(i) == 0) {

        printf("%d\n", i);

        /* int length = snprintf(NULL, 0, "%d", prime_numbers[i]); */
        /* char* str = malloc(length + 1); */
        /* snprintf(str, length + 1, "%d", prime_numbers[i]); */

        /* printf("%d\n", prime_numbers[i]); */

        /* fgets(str, sizeof(str), stdin); */
        /* fprintf(fptr, "%s", str); */
        /* fclose(fptr); */
      }
    };
  }

  return 0;
}


