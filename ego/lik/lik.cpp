#include "lik.h"

#include <ego/util/log/log.h>


namespace NEgo {

	TPredictiveDistributionParams ILik::CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const {
		TVectorD Y = NLa::Zeros(mean.n_rows);
		return CalculatePredictiveDistribution(Y, mean, variance);
	}

	TPredictiveDistributionParams ILik::CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const {
		TPredictiveDistributionParams ret;
		ret.LogP = CalculateLogLikelihood(Y, mean, variance).GetValue();
		Tie(ret.Mean, ret.Variance) = GetMarginalMeanAndVariance(mean, variance);
		return ret;
	}

	TDistrVec ILik::GetPredictiveDistributions(TPredictiveDistributionParams params, ui32 seed) {
		TDistrVec dvec;
        for(size_t pi=0; pi<params.Mean.size(); ++pi) {
            double mean = params.Mean(pi);
            double sd = sqrt(params.Variance(pi));
            dvec.push_back(GetDistribution(mean, sd, seed));
        }
        return dvec;
	}

	TLogLikValue ILik::CalculateLogLikelihood(const TVectorD &Y, const TVectorD &mean) const {
		TVectorD variance = NLa::Zeros(Y.n_rows);
		return CalculateLogLikelihood(Y, mean, variance);
	}

} // namespace NEgo