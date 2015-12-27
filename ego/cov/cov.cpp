#include "cov.h"


namespace NEgo {

	TCovRet ICov::CalculateKernel(const TMatrixD &m) {
		return CalculateKernel(m, m);
	}

} // namespace NEgo