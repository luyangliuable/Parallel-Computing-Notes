#include <stdio.h>

int main() {
  char text[10] = "rand";
  char copy[10];

  printf("%c\n", copy[0] = text[0]);
  printf("%c\n", copy[9]);

  if (copy[9] == '\0') {
    printf("true");
  }

  return 0;
}
