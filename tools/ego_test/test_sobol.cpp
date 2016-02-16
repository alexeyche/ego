#include <stdio.h>
#include <time.h>

/* test integrand from Joe and Kuo paper ... integrates to 1 */
static double testfunc(unsigned n, const double *x)
{
     double f = 1;
     unsigned j;
     for (j = 1; j <= n; ++j) {
      double cj = pow((double) j, 0.3333333333333333333);
      f *= (fabs(4*x[j-1] - 2) + cj) / (1 + cj);
     }
     return f;
}

int main(int argc, char **argv)
{
     unsigned n, j, i, sdim;
     static double x[MAXDIM];
     double testint_sobol = 0, testint_rand = 0;
     nlopt_sobol s;
     if (argc < 3) {
      fprintf(stderr, "Usage: %s <sdim> <ngen>\n", argv[0]);
      return 1;
     }
     nlopt_init_genrand(time(NULL));
     sdim = atoi(argv[1]);
     s = nlopt_sobol_create(sdim);
     n = atoi(argv[2]);
     nlopt_sobol_skip(s, n, x);
     for (j = 1; j <= n; ++j) {
      nlopt_sobol_next01(s, x);
      testint_sobol += testfunc(sdim, x);
      if (j < 100) {
           printf("x[%d]: %g", j, x[0]);
           for (i = 1; i < sdim; ++i) printf(", %g", x[i]);
           printf("\n");
      }
      for (i = 0; i < sdim; ++i) x[i] = nlopt_urand(0.,1.);
      testint_rand += testfunc(sdim, x);
     }
     nlopt_sobol_destroy(s);
     printf("Test integral = %g using Sobol, %g using pseudorandom.\n",
        testint_sobol / n, testint_rand / n);
     printf("        error = %g using Sobol, %g using pseudorandom.\n",
        testint_sobol / n - 1, testint_rand / n - 1);
     return 0;
}