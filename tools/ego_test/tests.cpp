#include "tests.h"




#include <ego/cov/stationary.h>
#include <ego/mean/const.h>
#include <ego/inf/exact.h>
#include <ego/lik/gauss.h>
#include <ego/acq/ei.h>
#include <ego/acq/lcb.h>
#include <ego/model/model.h>
#include <ego/util/sobol.h>

using namespace NKernels;

ONE_ARG_FUN_TEST(TMeanConst);

TWO_ARG_FUN_TEST(TSquareDistFunctor);

ONE_ARG_FUN_TEST(TSqExp);
ONE_ARG_FUN_TEST(TExp);
TWO_ARG_FUN_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TCovExpISO);

INF_TEST(TInfExact, TMeanConst, TCovSqExpISO, TLikGauss);
MODEL_TEST(TModel, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TAcqEI);
ACQ_TEST(TAcqEI, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TModel);
ACQ_TEST(TAcqLCB, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TModel);

TEST(SobolSequenceTest) {
    bool test_good = false;
    ui32 iter = 0;
    while ((!test_good) && iter < 5) {
        test_good = NSobolImpl::run_test(10, 10000, /*verbose = */ false);
        if (test_good) {
            L_DEBUG << "Try " << iter << ": sobol test passed";
        } else {
            L_DEBUG << "Try " << iter << ": sobol test failed";
        }
        ++iter;
    }

    ENSURE(test_good , "Sobol sequence failed in comparing with simple random 5 times");

    L_DEBUG << "Pass";
}