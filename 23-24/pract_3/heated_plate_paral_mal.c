#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
  #define M 500
  #define N 500

  double diff;
  double epsilon = 0.001;
  int i;
  int iterations;
  int iterations_print;
  int j;
  double mean;
  double my_diff;
  double u[M][N];
  double w[M][N];
  double wtime;

  printf("\n");
  printf("HEATED_PLATE\n");
  printf("  A program to solve for the steady state temperature distribution\n");
  printf("  over a rectangular plate.\n");
  printf("\n");
  printf("  Spatial grid of %d by %d points.\n", M, N);
  printf("  The iteration will be repeated until the change is <= %e\n", epsilon);

  mean = 0.0;

  // Set the boundary values, which don't change.
  #pragma omp parallel for private(i)
  for (i = 1; i < M - 1; i++) {
    w[i][0] = 100.0;
  }

  #pragma omp parallel for private(i)
  for (i = 1; i < M - 1; i++) {
    w[i][N - 1] = 100.0;
  }

  #pragma omp parallel for private(j)
  for (j = 0; j < N; j++) {
    w[M - 1][j] = 100.0;
  }

  #pragma omp parallel for private(j)
  for (j = 0; j < N; j++) {
    w[0][j] = 0.0;
  }

  // Average the boundary values, to come up with a reasonable initial value for the interior.
  #pragma omp parallel for reduction(+:mean) private(i, j)
  for (i = 1; i < M - 1; i++) {
    mean += w[i][0] + w[i][N - 1];
  }

  #pragma omp parallel for reduction(+:mean) private(j)
  for (j = 0; j < N; j++) {
    mean += w[M - 1][j] + w[0][j];
  }

  mean = mean / (double)(2 * M + 2 * N - 4);
  printf("\n");
  printf("  MEAN = %f\n", mean);

  // Initialize the interior solution to the mean value.
  #pragma omp parallel for private(i, j)
  for (i = 1; i < M - 1; i++) {
    for (j = 1; j < N - 1; j++) {
      w[i][j] = mean;
    }
  }

  iterations = 0;
  iterations_print = 1;
  printf("\n");
  printf(" Iteration  Change\n");
  printf("\n");

  diff = epsilon;

  while (epsilon <= diff) {
    // Save the old solution in U.
    #pragma omp parallel for private(i, j)
    for (i = 0; i < M; i++) {
      for (j = 0; j < N; j++) {
        u[i][j] = w[i][j];
      }
    }

    // Determine the new estimate of the solution at the interior points.
    #pragma omp parallel for private(i, j)
    for (i = 1; i < M - 1; i++) {
      for (j = 1; j < N - 1; j++) {
        w[i][j] = (u[i - 1][j] + u[i + 1][j] + u[i][j - 1] + u[i][j + 1]) / 4.0;
      }
    }

    // Compute the maximum difference between the new and old solution.
    
    diff = 0.0;
    #pragma omp parallel for private(i, j, my_diff) reduction(max:diff)
    for (i = 1; i < M - 1; i++) {
      for (j = 1; j < N - 1; j++) {
        my_diff = fabs(w[i][j] - u[i][j]);
        if (diff < my_diff) {
          diff = my_diff;
        }
      }
    }

    iterations++;
    if (iterations == iterations_print) {
      printf("  %8d  %f\n", iterations, diff);
      iterations_print = 2 * iterations_print;
    }
  }

  printf("\n");
  printf("  %8d  %f\n", iterations, diff);
  printf("\n");
  printf("  Error tolerance achieved.\n");
  printf("  Wallclock time = %f\n", wtime);

  printf("\n");
  printf("HEATED_PLATE_OPENMP:\n");
  printf("  Normal end of execution.\n");

  return 0;

  #undef M
  #undef N
}