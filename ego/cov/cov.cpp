#include "cov.h"


namespace NEgo {

    TMatrixD ICov::CalculateKernel(const TMatrixD &m) {
    	return CalculateKernel(m, m);
    }

} // namespace NEgo