#include "cov.h"

namespace NEgo {

	ICov::Result ICov::CrossCovariance(const TMatrixD &m) {
		return TParent::Calc(m, m);
	}

} // namespace NEgo