#include "tests.h"

#include <ego/cov/stationary.h>
#include <ego/mean/const.h>
#include <ego/inf/exact.h>
#include <ego/lik/gauss.h>
#include <ego/acq/ei.h>
#include <ego/model/model.h>

using namespace NKernels;

ONE_ARG_FUN_TEST(TSqExp);
TWO_ARG_FUN_TEST(TCovSqExpISO);
TWO_ARG_FUN_TEST(TSquareDistFunctor);
ONE_ARG_FUN_TEST(TMeanConst);
INF_TEST(TInfExact, TMeanConst, TCovSqExpISO, TLikGauss);

TEST(ModelTest) {
	TMatrixD X(5*SampleSize, DimSize, arma::fill::randn);
	TVectorD Y(5*SampleSize, arma::fill::randn);

	SPtr<IMean> mean = MakeShared(new TMeanConst(DimSize));
	SPtr<ICov> cov = MakeShared(new TCovSqExpISO(DimSize));
	SPtr<ILik> lik = MakeShared(new TLikGauss(DimSize));
	SPtr<IInf> inf = MakeShared(new TInfExact(mean, cov, lik));
	SPtr<IAcq> acq = MakeShared(new TAcqEI(DimSize));

	OneArgFunctorTester<TModel, SPtr<IMean>, SPtr<ICov>, SPtr<ILik>, SPtr<IInf>, SPtr<IAcq>, TMatrixD, TVectorD>(
		"ModelTest", mean, cov, lik, inf, acq, X, Y
	);
}
