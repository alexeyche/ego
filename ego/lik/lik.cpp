#include "lik.h"


namespace NEgo {

    TDistrVec ILik::GetPredictiveDistributions(const TVectorD& mean, const TVectorD& variance, ui32 seed) {
		TDistrVec dvec;
        for(size_t pi=0; pi<mean.size(); ++pi) {
            const double& var = variance(pi);
            ENSURE(var>=0.0, "Got negative variance, something wrong in system");
            dvec.push_back(GetDistribution(mean(pi), sqrt(var), seed));
        }
        return dvec;
	}

    TDistrVec ILik::GetPredictiveDistributionsWithDerivative(
        const TVectorD& mean, const TVectorD& variance,
        const TVectorD& meanDeriv, const TVectorD& varianceDeriv, ui32 seed)
    {
        TDistrVec dvec;
        for(size_t pi=0; pi<mean.size(); ++pi) {
            const double& var = variance(pi);
            ENSURE(var>=0.0, "Got negative variance, something wrong in system");
            double sd = sqrt(var);
            SPtr<IDistr> d = GetDistribution(
                mean(pi)
              , sd
              , seed
            );
            d->Set
        meanDeriv(pi)
              , 0.5 * varianceDeriv(pi) / sd
            dvec.push_back(d);
        }
        return dvec;
    }


} // namespace NEgo