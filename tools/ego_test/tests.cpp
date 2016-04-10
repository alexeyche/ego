#include "test_framework.h"

#include <ego/cov/stationary_iso.h>
#include <ego/cov/stationary_ard.h>
#include <ego/mean/const.h>
#include <ego/inf/exact.h>
#include <ego/lik/gauss.h>
#include <ego/acq/ei.h>
#include <ego/acq/lcb.h>
#include <ego/model/model.h>
#include <ego/model/tree_model.h>

using namespace NKernels;

ONE_ARG_FUN_TEST(TMeanConst);

TWO_ARG_FUN_TEST(TSquareDistFunctor);

ONE_ARG_FUN_TEST(TSqExp);
ONE_ARG_FUN_TEST(TExp);

TWO_ARG_FUN_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TCovExpISO);
TWO_ARG_FUN_TEST(TCovMatern32ISO);
TWO_ARG_FUN_TEST(TCovMatern52ISO);

TWO_ARG_FUN_TEST(TCovSqExpARD);
TWO_ARG_FUN_TEST(TCovExpARD);
TWO_ARG_FUN_TEST(TCovMatern32ARD);
TWO_ARG_FUN_TEST(TCovMatern52ARD);

INF_TEST(TInfExact, TMeanConst, TCovMatern52ARD, TLikGauss);
MODEL_TEST(TModel, TMeanConst, TCovMatern52ARD, TLikGauss, TInfExact, TAcqEI, LilEpsilon);
ACQ_TEST(TAcqEI, TMeanConst, TCovMatern52ARD, TLikGauss, TInfExact, TModel);
ACQ_TEST(TAcqLCB, TMeanConst, TCovMatern52ARD, TLikGauss, TInfExact, TModel);
MODEL_TEST(TTreeModel, TMeanConst, TCovMatern52ARD, TLikGauss, TInfExact, TAcqEI, LilEpsilon*10.0);