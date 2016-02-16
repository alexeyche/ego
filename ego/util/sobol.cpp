#include "sobol.h"

#include <chrono>

namespace NEgo {

    using namespace NSobolImpl;

    TMatrixD GenerateSobolGrid(ui32 samplesNum, ui32 dimSize) {
        TMatrixD grid(samplesNum, dimSize);

        NSobolImpl::soboldata_s* s;
        s = NSobolImpl::sobol_create(dimSize);

        double *x = new double[dimSize];
        NSobolImpl::sobol_skip(s, samplesNum, x);
        for (ui32 i = 0; i < samplesNum; ++i) {
            NSobolImpl::sobol_next01(s, x);
            for (ui32 j = 0; j < dimSize; ++j) {
                grid(i, j) = x[j];
            }
        }
        NSobolImpl::sobol_destroy(s);
        delete []x;

        return grid;
    }


    namespace NSobolImpl {

        /* Return position (0, 1, ...) of rightmost (least-significant) zero bit in n.
         *
         * This code uses a 32-bit version of algorithm to find the rightmost
         * one bit in Knuth, _The Art of Computer Programming_, volume 4A
         * (draft fascicle), section 7.1.3, "Bitwise tricks and
         * techniques."
         *
         * Assumes n has a zero bit, i.e. n < 2^32 - 1.
         *
         */
        static unsigned rightzero32(uint32_t n)
        {
        #if defined(__GNUC__) && \
            ((__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || __GNUC__ > 3)
             return __builtin_ctz(~n); /* gcc builtin for version >= 3.4 */
        #else
             const uint32_t a = 0x05f66a47; /* magic number, found by brute force */
             static const unsigned decode[32] = {0,1,2,26,23,3,15,27,24,21,19,4,12,16,28,6,31,25,22,14,20,18,11,5,30,13,17,10,29,9,8,7};
             n = ~n; /* change to rightmost-one problem */
             n = a * (n & (-n)); /* store in n to make sure mult. is 32 bits */
             return decode[n >> 27];
        #endif
        }

        /* generate the next term x_{n+1} in the Sobol sequence, as an array
           x[sdim] of numbers in (0,1).  Returns 1 on success, 0 on failure
           (if too many #'s generated) */
        static int sobol_gen(soboldata *sd, double *x)
        {
             unsigned c, b, i, sdim;

             if (sd->n == 4294967295U) return 0; /* n == 2^32 - 1 ... we would
                                need to switch to a 64-bit version
                                to generate more terms. */
             c = rightzero32(sd->n++);
             sdim = sd->sdim;
             for (i = 0; i < sdim; ++i) {
              b = sd->b[i];
              if (b >= c) {
                   sd->x[i] ^= sd->m[c][i] << (b - c);
                   x[i] = ((double) (sd->x[i])) / (1U << (b+1));
              }
              else {
                   sd->x[i] = (sd->x[i] << (c - b)) ^ sd->m[c][i];
                   sd->b[i] = c;
                   x[i] = ((double) (sd->x[i])) / (1U << (c+1));
              }
             }
             return 1;
        }

        #include "soboldata.h"

        static int sobol_init(soboldata *sd, unsigned sdim)
        {
             unsigned i,j;

             if (!sdim || sdim > MAXDIM) return 0;

             sd->mdata = (uint32_t *) malloc(sizeof(uint32_t) * (sdim * 32));
             if (!sd->mdata) return 0;

             for (j = 0; j < 32; ++j) {
              sd->m[j] = sd->mdata + j * sdim;
              sd->m[j][0] = 1; /* special-case Sobol sequence */
             }
             for (i = 1; i < sdim; ++i) {
              uint32_t a = sobol_a[i-1];
              unsigned d = 0, k;

              while (a) {
                   ++d;
                   a >>= 1;
              }
              d--; /* d is now degree of poly */

              /* set initial values of m from table */
              for (j = 0; j < d; ++j)
                   sd->m[j][i] = sobol_minit[j][i-1];

              /* fill in remaining values using recurrence */
              for (j = d; j < 32; ++j) {
                   a = sobol_a[i-1];
                   sd->m[j][i] = sd->m[j - d][i];
                   for (k = 0; k < d; ++k) {
                    sd->m[j][i] ^= ((a & 1) * sd->m[j-d+k][i]) << (d-k);
                    a >>= 1;
                   }
              }
             }

             sd->x = (uint32_t *) malloc(sizeof(uint32_t) * sdim);
             if (!sd->x) { free(sd->mdata); return 0; }

             sd->b = (unsigned *) malloc(sizeof(unsigned) * sdim);
             if (!sd->b) { free(sd->x); free(sd->mdata); return 0; }

             for (i = 0; i < sdim; ++i) {
              sd->x[i] = 0;
              sd->b[i] = 0;
             }

             sd->n = 0;
             sd->sdim = sdim;

             return 1;
        }

        static void sobol_destroy_inner(soboldata *sd)
        {
             free(sd->mdata);
             free(sd->x);
             free(sd->b);
        }

        /************************************************************************/
        /* NLopt API to Sobol sequence creation, which hides soboldata structure
           behind an opaque pointer */

        soboldata_s* sobol_create(unsigned sdim)
        {
             soboldata_s* s = (soboldata_s*) malloc(sizeof(soboldata));
             if (!s) return NULL;
             if (!sobol_init(s, sdim)) { free(s); return NULL; }
             return s;
        }

        extern void sobol_destroy(soboldata_s* s)
        {
             if (s) {
              sobol_destroy_inner(s);
              free(s);
             }
        }

        /* next vector x[sdim] in Sobol sequence, with each x[i] in (0,1) */
        void sobol_next01(soboldata_s* s, double *x)
        {
             if (!sobol_gen(s, x)) {
              /* fall back on pseudo random numbers in the unlikely event
                 that we exceed 2^32-1 points */


                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(0.0, 1.0);
                unsigned i;
                for (i = 0; i < s->sdim; ++i)
                    x[i] = dis(gen);
                }
        }

        /* next vector in Sobol sequence, scaled to (lb[i], ub[i]) interval */
        void sobol_next(soboldata_s* s, double *x,
                      const double *lb, const double *ub)
        {
             unsigned i, sdim;
             sobol_next01(s, x);
             for (sdim = s->sdim, i = 0; i < sdim; ++i)
              x[i] = lb[i] + (ub[i] - lb[i]) * x[i];
        }

        /* if we know in advance how many points (n) we want to compute, then
           adopt the suggestion of the Joe and Kuo paper, which in turn
           is taken from Acworth et al (1998), of skipping a number of
           points equal to the largest power of 2 smaller than n */
        void sobol_skip(soboldata_s* s, unsigned n, double *x)
        {
             if (s) {
              unsigned k = 1;
              while (k*2 < n) k *= 2;
              while (k-- > 0) sobol_gen(s, x);
             }
        }

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

        bool run_test(unsigned sdim, unsigned n, bool verbose) {
            unsigned j, i;
             static double x[MAXDIM];
             double testint_sobol = 0, testint_rand = 0;
             soboldata_s* s;
             s = sobol_create(sdim);
             sobol_skip(s, n, x);

             std::random_device rd;
             std::mt19937 gen(rd());
             gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
             std::uniform_real_distribution<> dis(0.0, 1.0);

             for (j = 1; j <= n; ++j) {
              sobol_next01(s, x);
              testint_sobol += testfunc(sdim, x);
              if (verbose) {
                  if (j < 100) {
                       printf("x[%d]: %g", j, x[0]);
                       for (i = 1; i < sdim; ++i) printf(", %g", x[i]);
                       printf("\n");
                  }
              }

              for (i = 0; i < sdim; ++i) x[i] = dis(gen);
              testint_rand += testfunc(sdim, x);
             }
             sobol_destroy(s);
             printf("Test integral = %g using Sobol, %g using pseudorandom.\n",
                testint_sobol / n, testint_rand / n);
             printf("        error = %g using Sobol, %g using pseudorandom.\n",
                testint_sobol / n - 1, testint_rand / n - 1);


            return std::abs(testint_sobol / n - 1) < std::abs(testint_rand / n - 1);
        }



    } // namespace NSobolImpl

} // namespace NEgo