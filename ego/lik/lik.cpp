#include "lik.h"


namespace NEgo {


	TPredictiveDistributionParams ILik::CalculatePredictiveDistribution(const TVectorD &mean, const TVectorD &variance) const {
		TVectorD Y = NLa::Zeros(mean.n_rows);
		return CalculatePredictiveDistribution(Y, mean, variance);
	}


	TPredictiveDistributionParams ILik::CalculatePredictiveDistribution(const TVectorD &Y, const TVectorD &mean, const TVectorD &variance) const {
		TPredictiveDistributionParams ret;
		ret.LogP = Calc(Y-mean, variance).Value();
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


} // namespace NEgo