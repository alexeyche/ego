#include "test_framework.h"

#include <ego/cov/stationary_iso.h>
#include <ego/cov/stationary_ard.h>
#include <ego/mean/const.h>
#include <ego/inf/exact.h>
#include <ego/lik/gauss.h>
#include <ego/acq/ei.h>
#include <ego/acq/lcb.h>
#include <ego/model/model.h>

using namespace NKernels;

ONE_ARG_FUN_TEST(TMeanConst);

TWO_ARG_FUN_TEST(TSquareDistFunctor);

ONE_ARG_FUN_TEST(TSqExp);
ONE_ARG_FUN_TEST(TExp);
TWO_ARG_FUN_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TCovExpISO);
TWO_ARG_FUN_TEST(TCovSqExpARD);

INF_TEST(TInfExact, TMeanConst, TCovSqExpISO, TLikGauss);
MODEL_TEST(TModel, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TAcqEI);
ACQ_TEST(TAcqEI, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TModel);
ACQ_TEST(TAcqLCB, TMeanConst, TCovSqExpISO, TLikGauss, TInfExact, TModel);
