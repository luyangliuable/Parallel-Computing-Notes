#include <memory.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    // Read display.txt into numbers array
    FILE *fp;
    int i;
    fp = fopen("display.txt", "r");
    char cr;
    int lines = 0;
    int wins = 0;
    while (cr != EOF) {
        if (cr == '\n') {
            lines++;
        }
        cr = getc(fp);
    }
    rewind(fp);

    int numbers[lines];
    for (i = 0; i < lines; i++) {
        fscanf(fp, "%d\n", &numbers[i]);
    }

    fclose(fp);

    // Create array to count if digits has already been found
    int digits[10];
    for (int i = 0; i < 10; i++) {
        digits[i] = 0;
    }

// Check matching entries in parallel
#pragma omp parallel
    {
        int i;
        int my_rank = omp_get_thread_num();
        int num_threads = omp_get_num_threads();  // change to p

        int rpt = lines / num_threads;
        int rptr = lines % num_threads;

        int sp = my_rank * rpt;
        int ep = sp + rpt;

        if (my_rank == num_threads - 1) ep += rptr;

        for (int i = sp; i < ep; i++) {
#pragma omp critical
            digits[numbers[i]] += 1;
            printf("Found: %d", numbers[i]);
        }
    }
    // check occurrences for wins
    for (int i = 0; i < 10; i++) {
        if (digits[i] > 1) wins += 1;
        printf("Digit: %d Number of occurrences: %d\n", i, digits[i]);
    }
    fp = fopen("wins.txt", "w");
    fprintf(fp, "%d\n", wins);
    printf("Number of wins: %d\n", wins);
    fclose(fp);
    return (0);
}
