#include "tests.h"

#include <ego/cov/stationary.h>
#include <ego/mean/const.h>
#include <ego/inf/exact.h>
#include <ego/lik/gauss.h>

using namespace NKernels;

ONE_ARG_FUN_TEST(TSqExp);
TWO_ARG_FUN_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TSquareDistFunctor);
ONE_ARG_FUN_TEST(TMeanConst);
INF_TEST(TInfExact, TMeanConst, TCovSqExpISO, TLikGauss);
