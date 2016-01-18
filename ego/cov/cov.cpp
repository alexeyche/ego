#include "cov.h"


namespace NEgo {

	ICov::Result ICov::CrossCovariance(const TMatrixD &m) {
		return TParent::Calc(m, m);
	}

	TVector ICov::Sample(const TMatrixD &x) {
		return NLa::SampleMultivariateGauss(NLa::Zeros())
	}

} // namespace NEgo