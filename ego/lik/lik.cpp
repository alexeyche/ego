#include "lik.h"

namespace NEgo {

	TPredictiveDistribution ILik::CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const {
		TVectorD Y(mean.n_rows);
		return CalculatePredictiveDistribution(Y, mean, variance);
	}

} // namespace NEgo