#pragma once

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <random>
#include <cmath>

#include <ego/base/errors.h>
#include <ego/base/la.h>

namespace NEgo {

    namespace NSobolImpl {

        typedef struct soboldata_s {
             unsigned sdim; /* dimension of sequence being generated */
             uint32_t *mdata; /* array of length 32 * sdim */
             uint32_t *m[32]; /* more convenient pointers to mdata, of direction #s */
             uint32_t *x; /* previous x = x_n, array of length sdim */
             unsigned *b; /* position of fixed point in x[i] is after bit b[i] */
             uint32_t n; /* number of x's generated so far */
        } soboldata;

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
        static unsigned rightzero32(uint32_t n);

        /* generate the next term x_{n+1} in the Sobol sequence, as an array
           x[sdim] of numbers in (0,1).  Returns 1 on success, 0 on failure
           (if too many #'s generated) */
        static int sobol_gen(soboldata *sd, double *x);

        static int sobol_init(soboldata *sd, unsigned sdim);

        static void sobol_destroy_inner(soboldata *sd);

        /************************************************************************/

        soboldata_s* sobol_create(unsigned sdim);

        extern void sobol_destroy(soboldata_s* s);

        /* next vector x[sdim] in Sobol sequence, with each x[i] in (0,1) */
        void sobol_next01(soboldata_s* s, double *x);

        /* next vector in Sobol sequence, scaled to (lb[i], ub[i]) interval */
        void sobol_next(soboldata_s* s, double *x,
                      const double *lb, const double *ub);

        /* if we know in advance how many points (n) we want to compute, then
           adopt the suggestion of the Joe and Kuo paper, which in turn
           is taken from Acworth et al (1998), of skipping a number of
           points equal to the largest power of 2 smaller than n */
        void sobol_skip(soboldata_s* s, unsigned n, double *x);

        /* test integrand from Joe and Kuo paper ... integrates to 1 */
        static double testfunc(unsigned n, const double *x);

        bool run_test(unsigned sdim, unsigned n, bool verbose);

    } // namespace NSobolImpl

    TMatrixD GenerateSobolGrid(ui32 samplesNum, ui32 dimSize, double min = 0.0, double max = 1.0);

    class TSobolGen {
    public:
        TSobolGen()
            : SobolData(nullptr)
        {}

        void Init(ui32 dimSize) {
            DimSize = dimSize;
            SobolData = NSobolImpl::sobol_create(DimSize);
        }

        TSobolGen(ui32 dimSize)
        {
            Init(dimSize);
        }

        TMatrixD Sample(ui32 samplesNum, double min=0.0, double max=1.0) {
            ENSURE(SobolData, "Sobol generator was not initialized");

            TMatrixD grid(samplesNum, DimSize);

            double *x = new double[DimSize];
            NSobolImpl::sobol_skip(SobolData, samplesNum, x);
            for (ui32 i = 0; i < samplesNum; ++i) {
                NSobolImpl::sobol_next01(SobolData, x);
                for (ui32 j = 0; j < DimSize; ++j) {
                    grid(i, j) = min + x[j] * (max - min);
                }
            }
            return grid;
        }

        TSobolGen(const TSobolGen& another) {
            (*this) = another;
        }

        TSobolGen& operator = (const TSobolGen& another) {
            if (this != &another) {
                if (SobolData) {
                    NSobolImpl::sobol_destroy(SobolData);
                }
                    
                if (another.SobolData) {
                    Init(another.DimSize);
                    Sample(another.SobolData->n);    
                }
            }
            return *this;
        }

        ~TSobolGen() {
            NSobolImpl::sobol_destroy(SobolData);
        }

    private:
        ui32 DimSize;
        NSobolImpl::soboldata_s* SobolData;   
    };



} // namespace NEgo