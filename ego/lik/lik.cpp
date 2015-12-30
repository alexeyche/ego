#include "lik.h"

namespace NEgo {

	TPredictiveDistributionParams ILik::CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const {
		TVectorD Y(mean.n_rows);
		return CalculatePredictiveDistribution(Y, mean, variance);
	}


	TDistrVec ILik::GetPredictiveDistributions(TPredictiveDistributionParams params, ui32 seed) {
		TDistrVec dvec;
        for(size_t pi=0; pi<params.Mean.size(); ++pi) {
            double mean = params.Mean(pi);
            double sd = params.Variance(pi);
            dvec.push_back(GetDistribution(mean, sd, seed));
        }
        return dvec;
	}

} // namespace NEgo