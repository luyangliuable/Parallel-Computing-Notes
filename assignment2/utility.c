#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define pi 3.14159265358979323846

typedef struct {
  double lat;
  double lon;
} location;


double compute_absolute_diff(double value1, double value2) {
  double res = fabs(value1 - value2);
  return res;
}

double r(double var) {
  double value = (int)(var * 100 + .5);
  return (double)value / 100;
}

/* double compute_distance(int *coord1, int *coord2) { */
/*   double res = */
/*       sqrt(pow(coord1[1] - coord2[1], 2) + pow(coord1[0] - coord2[0], 2)); */
/*   return res; */
/* } */

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  Function prototypes                                           :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double);
double rad2deg(double);

double distance(double lat1, double lon1, double lat2, double lon2, char unit) {
  double theta, dist;
  if ((lat1 == lat2) && (lon1 == lon2)) {
    return 0;
  } else {
    theta = lon1 - lon2;
    dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) +
           cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * cos(deg2rad(theta));
    dist = acos(dist);
    dist = rad2deg(dist);
    dist = dist * 60 * 1.1515;
    switch (unit) {
    case 'M':
      break;
    case 'K':
      dist = dist * 1.609344;
      break;
    case 'N':
      dist = dist * 0.8684;
      break;
    }
    return (dist);
  }
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts decimal degrees to radians             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double deg2rad(double deg) { return (deg * pi / 180); }

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
/*::  This function converts radians to decimal degrees             :*/
/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
double rad2deg(double rad) { return (rad * 180 / pi); }

void locaiton_to_arr(double *arr, location location) {
  arr[0] = location.lat;
  arr[1] = location.lat;
}

location arr_to_location(double *arr) {
  location res;
  res.lat = arr[0];
  res.lon = arr[1];

  return res;
}

int check_same(location location1, location location2, double mag1, double mag2, int d) {
  if (round(location1.lat) == location2.lat &&
      location1.lon == location2.lon && mag1 == mag2) {
    return 1;
  }

  return 0;
}

/* int main(int argc, char *argv[]) { */
/*   double coord1[2] = {1.2, 1.5}; */
/*   double coord2[2] = {1.2, 1.5}; */
/*   location loc1 = arr_to_location(coord1); */
/*   location loc2 = arr_to_location(coord2); */
/*   int same = check_same(loc1, loc2, 2, 2); */

/*   if (same) { */
/*     printf("Same.\n"); */
/*   } */
/*   /\* double dist = distance(coord1[0], coord1[1], coord2[0], coord2[1],
 * 'K'); *\/ */
/*   /\*   double distance = compute_distance(coord1, coord2); *\/ */
/*   /\* printf("(%d, %d): Distance with (%d, %d) is %.2f\n", coord1[0],
 * coord1[1], */
/*    *\/ */
/*   /\*        coord2[0], coord2[1], dist); *\/ */
/*   return 0; */
/* } */
