#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

float calculate_d(int a, int b, int c) {
  float d = pow(b, 2) - 4 * a * c;
  return d;
}

typedef struct {
  float real;
  float img;
} root;

root *quadratic_root(int a, int b, int d) {
  root *roots = malloc(2 * sizeof(root));

  if (d > 0) {
    roots[0].real = (-b + sqrt(d)) / (2 * a);
    roots[0].img = 0;
    roots[1].real = (-b - sqrt(d)) / (2 * a);
    roots[1].img = 0;
  } else if (d < 0) {
    float nom = -b;
    float denom = 2 * a;
    roots[0].real = nom / denom;
    roots[0].img = sqrt(abs(d)) / (2 * a);
    nom = -b;
    denom = 2 * a;
    roots[1].real = nom / denom;
    roots[1].img = -sqrt(abs(d)) / (2 * a);
  }

  return roots;
}
