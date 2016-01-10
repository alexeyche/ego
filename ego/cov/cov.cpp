#include "cov.h"


namespace NEgo {

	ICov::Result ICov::CrossCovariance(const TMatrixD &m) {
		return Parent::Calc(m, m);
	}

} // namespace NEgo